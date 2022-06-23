#include "rabbitmq_writer.h"
#ifdef _WIN32
#include "GZipCodec/GZipCodec.h"
#else
#include "gzip_codec/gzip_codec.h"
#endif

#include <common_fun/fun_coding.h>
#include "logging.h"
#include "test_code.h"
#include <boost/filesystem.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/list.hpp>

RabbitMQWriter::RabbitMQWriter(const Config& config)
    : config_(config)
{
	load();
    thread_ = std::thread(&RabbitMQWriter::DoWork, this);
}

RabbitMQWriter::~RabbitMQWriter()
{
    exit_ = true;
	cv_.notify_one();
    thread_.join();
	save();
}

Channel::ptr_t RabbitMQWriter::RecreateChannelIfNeeded(Channel::ptr_t &channel)
{
    if (channel == NULL)
    {
        try
        {
            channel = Channel::Create(config_.address, config_.port, config_.user, config_.password, config_.vhost);
			channel->DeclareExchange(config_.exchange, Channel::EXCHANGE_TYPE_TOPIC, false, true, false);
		}
        catch (std::runtime_error const &e)
        {
            LOG_ERROR << __FUNCTION__ << " exception: " << e.what();
            channel.reset();
        }
    }
    return channel;
}

void RabbitMQWriter::DoWork()
{
    while (!exit_)
    {
		{
			std::unique_lock<std::mutex> _(mutex_);
			cv_.wait(_, [this]() {
				return !list_.empty() || exit_;
			});
		}

		if (exit_)
		{
			return;
		}

		channel_ = RecreateChannelIfNeeded(channel_);
		if (!channel_)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			continue;
		}

		list<string> tmp_list;
		{
			std::lock_guard<std::mutex> _(mutex_);
			tmp_list.swap(list_);
		}
		
		for (auto itr = tmp_list.begin(); itr != tmp_list.end(); ++itr)
		{
			if (exit_)
			{
				std::lock_guard<std::mutex> _(mutex_);
				list_.insert(list_.begin(), itr, tmp_list.end());
				break;
			}

			bool failed = false;
			try
			{
				BasicMessage::ptr_t message = BasicMessage::Create(*itr);
				channel_->BasicPublish(config_.exchange, config_.routing_key, message);
			}
			catch (const ConnectionException &e)
			{
				failed = true;
				LOG_ERROR << __FUNCTION__ << " exception: " << e.what();
			}
			catch (const AmqpResponseLibraryException  &e)
			{
				failed = true;
				LOG_ERROR << __FUNCTION__ << " exception: " << e.what();
			}
			catch (const std::runtime_error &e)
			{
				failed = true;
				LOG_ERROR << __FUNCTION__ << " exception: " << e.what();
			}
			catch (exception &e)
			{
				failed = true;
				LOG_ERROR << __FUNCTION__ << " exception: " << e.what();
			}

			if (failed)
			{
				channel_.reset();
				{
					std::lock_guard<std::mutex> _(mutex_);
					list_.insert(list_.begin(), itr, tmp_list.end());
				}
				break;
			}
		}
    }
}

void RabbitMQWriter::produce(const string& str)
{
	if (config_.zip)
	{
		std::string compress_data, message;
#ifdef _WIN32
		GZipCompress(str, compress_data);
#else
		e6::gzip_codec::compress(str, compress_data);
#endif
		message = EncodeBase64(compress_data);
		push(message);
	}
	else
	{
		push(str);
	}
}

void RabbitMQWriter::push(const string& str)
{
	std::lock_guard<std::mutex> _(mutex_);
	list_.push_back(str);
	cv_.notify_one();
}

void RabbitMQWriter::save()
{
	auto file_name = getFileName();
	ofstream file_objid(file_name, std::ios::binary);

	if (!file_objid.fail())
	{
		decltype(list_) tmp_list;
		{
			std::lock_guard<std::mutex> _(mutex_);
			tmp_list = list_;
		}

		try
		{
			boost::archive::binary_oarchive bin_oar(file_objid);
			bin_oar << tmp_list;
		}
		catch (boost::archive::archive_exception &)
		{
			file_objid.close();
			return;
		}
		file_objid.close();
	}
}

void RabbitMQWriter::load()
{
	auto file_name = getFileName();
	ifstream file_objid(file_name, std::ios::binary);
	if (!file_objid.fail())
	{
		try
		{
			boost::archive::binary_iarchive bin_iar(file_objid);
			bin_iar >> list_;
		}
		catch (boost::archive::archive_exception &)
		{
			//�ļ���ȡ����ʱ���� 1.ɾ���ļ�
			//ɾ���ļ�ǰ �ȹر��ļ�
			file_objid.close();
			boost::filesystem::remove(file_name);
			return;
		}
		catch (...)
		{
			file_objid.close();
			//LogLocalText(LOG_LOCAL_ERROR_ERROR, "DevParamMgr::load() fail");
			//printf_s("DevParamMgr::load() fail  ");
			assert(0);
			return;
		}

		file_objid.close();
	}
	else
	{
		boost::filesystem::remove(file_name);
	}
}

string RabbitMQWriter::getFileName()
{
	char tmp[128];
	sprintf(tmp, "app_data//exchange_%s.dat", config_.exchange.c_str());
	return tmp;
}