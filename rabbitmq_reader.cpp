#include "rabbitmq_reader.h"
#ifdef _WIN32
#include "GZipCodec/GZipCodec.h"
#else
#include "gzip_codec/gzip_codec.h"
#endif

#include <common_fun/fun_coding.h>
#include "logging.h"
#include "boost/locale.hpp"
#include "test_code.h"

RabbitMQReader::RabbitMQReader(Config const &config, CallBack callback)
    : config_(config), callback_(callback)
{
    thread_ = std::thread(&RabbitMQReader::DoWork, this);
}

RabbitMQReader::~RabbitMQReader()
{
    exit_ = true;
	cv_.notify_one();
    thread_.join();
}

Channel::ptr_t RabbitMQReader::RecreateChannelIfNeeded(Channel::ptr_t &channel)
{
    if (channel == NULL)
    {
        try
        {
            channel = Channel::Create(config_.address, config_.port, config_.user, config_.password, config_.vhost);
            Initialize(channel);
        }
        catch (std::runtime_error const &e)
        {
            LOG_ERROR << __FUNCTION__ << " exception: " << e.what();
            // ignore
            //LogLocalText(LOG_LOCAL_ERROR_ERROR, "exception at %s : %s", __FUNCTION__, e.what());
            channel.reset();
        }
    }
    return channel;
}

int32_t RabbitMQReader::Initialize(const Channel::ptr_t &channel)
{
	if (config_.auto_delete)
	{
		channel->DeleteQueue(config_.queue,true);
	}
	channel->DeclareQueue(config_.queue, false, true, false, config_.auto_delete);
	channel->DeclareExchange(config_.exchange, "topic", false, true, false);
	channel->BindQueue(config_.queue, config_.exchange, config_.routing_key);
    // create consumer
    {
        consumer_tag_ = channel->BasicConsume(config_.queue);
        LOG_INFO << "RabbitMQ create tag: " << consumer_tag_;
    }

    return 1;
}

void RabbitMQReader::DoWork()
{
    while (!exit_)
    {
		std::unique_lock<std::mutex> _(mutex_);
		cv_.wait(_, [this]() {
			return !lock_ || exit_;
		});

		if (exit_)
		{
			return;
		}

        if(Consume())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        }
    }

    return;
}

int32_t RabbitMQReader::Consume()
{
    channel_ = RecreateChannelIfNeeded(channel_);
    if(!channel_)
    {
        return 0;
    }

    bool failed = false;
    try
    {
        Envelope::ptr_t envelope;
        while (!exit_ && channel_->BasicConsumeMessage(consumer_tag_, envelope,1000))
        {
            if (callback_)
            {
                BasicMessage::ptr_t message = envelope->Message();
                std::string message_body = message->Body();

                std::string message_uncompressed, message_decoded;
                if (config_.zip)
                {
					message_decoded = DecodeBase64(message_body);
#ifdef _WIN32
					GZipUncompress(message_decoded, message_uncompressed);
#else
					e6::gzip_codec::decompress(message_decoded, message_uncompressed);
#endif      
                }
                else
                {
                    message_uncompressed.swap(message_body);
                }
                //string utf = boost::locale::conv::between(message_uncompressed.c_str(), "UTF-8", "GBK");

                //LOG_INFO << __FUNCTION__ << " callback message: " << utf;
                callback_(config_.queue, message_uncompressed);
            }
        }
    }
    catch (const ConnectionException &e)
    {
		LOG_ERROR << "rabbitmq read ConnectionException at" << __FUNCTION__ << " : " << e.what();
        failed = true;
    }
    catch (const AmqpResponseLibraryException &e)
    {
		LOG_ERROR << "rabbitmq read AmqpResponseLibraryException at" << __FUNCTION__ << " : " << e.what();
        failed = true;
    }
    catch (const std::runtime_error &e)
    {
		LOG_ERROR << "rabbitmq read runtime_error at" << __FUNCTION__ << " : " << e.what();
        failed = true;
    }

    if (failed)
    {
        channel_.reset();
    }
    return 1;
}

void RabbitMQReader::lock()
{
	lock_ = true;
}

void RabbitMQReader::unlock()
{
	lock_ = false;
	cv_.notify_one();
}