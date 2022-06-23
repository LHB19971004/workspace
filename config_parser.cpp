#include "config_parser.h"
#include "utils.h"
#include "logging.h"

std::shared_ptr<ConfigItem> ConfigParser::parse()
{
	std::shared_ptr<ConfigItem> config;
	if (tokenize())
	{
		std::shared_ptr<Token> token;
		while (token = peekToken())
		{
			auto configItem = parseConfigItem();
			if (!config)
			{
				config = std::make_shared<ConfigItem>("__root__");
			}

			config->pushArgument(configItem);
		};
	}
	return config;
}

std::shared_ptr<ConfigItem> ConfigParser::parseConfigItem()
{
	
	std::shared_ptr<Token> token;
	std::shared_ptr<ConfigItem> configItem = nullptr;
	while ((token = peekToken()) /*!= nullptr */ && token->is(Token::Symbol))
	{
		if (!configItem)
		{
			configItem = std::make_shared<ConfigItem>(token->content());
		}
		else
		{
			configItem->pushArgument(token->content());
		}
		nextToken();
	}

	bool parseBlock = false;
	if (configItem && (token = peekToken()) && token->is(Token::Sign, "{"))
	{
		nextToken();
		parseBlock = true;
		while (configItem && (token = peekToken()) && !token->is(Token::Sign, "}"))
		{
			auto arg = parseConfigItem();
			configItem->pushArgument(arg);
		}
	}

	auto endingCh = parseBlock ? "}" : ";";
	if ((token = peekToken()) && token->is(Token::Sign, endingCh))
	{
		nextToken();
	}
	else
	{
		LOG_ERROR << "ConfigParser::parseConfigItem : expect" << endingCh << " near " << configItem->text();
		return nullptr;
	}

	return configItem;
}

bool ConfigParser::tokenize()
{
	auto readComment = [this]() -> std::shared_ptr<Token>
	{
		std::string str;
		char ch;
		if (!next(ch) && ch != '#')
		{
			return nullptr;
		}

		while (peek(ch) && (ch != '\r' && ch != '\n'))
		{
			str += ch;
			next(ch);
		}

		return std::make_shared<Token>(Token::Comment, str);
	};

	auto readString = [this]() ->std::shared_ptr<Token>
	{
		bool enclosed = false;
		std::string str;
		char ch;
		if (!peek(ch))
		{
			return nullptr;
		}
		if (ch == '\"')
		{
			enclosed = true;
			next(ch);
		}

		if (!enclosed)
		{
			while (peek(ch) && !isspace(ch) && ch != ';' && ch != '{' && ch != '}' )
			{
				str += ch;
				next(ch);
			}
		}
		else
		{
			while (peek(ch) && ch != '\"')
			{
				str += ch;
				next(ch);
			}

			if (peek(ch) && ch == '\"')
			{
				next(ch);
			}
		}
		return std::make_shared<Token>(Token::Symbol, str);
	};

	char ch;
	while (peek(ch))
	{
		if (isspace(ch))
		{
			next(ch);
			continue;
		}

		std::shared_ptr<Token> token;
		switch (ch)
		{
		case '#':
			token = readComment();
			break;
		case ';':
		case '{':
		case '}':
			token = std::make_shared<Token>(Token::Sign, std::string(1, ch));
			next(ch);
			break;
		case '"':
		default:
			token = readString();
			break;
		}

		if (token && !token->is(Token::Comment))
		{
			tokens_.push_back(token);
		}
	}
	return true;
}

bool ConfigParser::next(char &ch, bool adv/* = true*/)
{
	if (offset_ < content_.size())
	{
		ch = content_[adv ? offset_++ : offset_];
		return true;
	}
	return false;
}

bool ConfigParser::peek(char & ch)
{
	return next(ch, false);
}


std::shared_ptr<ConfigParser::Token> ConfigParser::nextToken(bool adv/* = true*/)
{
	std::shared_ptr<ConfigParser::Token> ptr = nullptr;
	if (tokenOffset_ < tokens_.size())
	{
		ptr = tokens_[adv ? tokenOffset_++ : tokenOffset_];
	}
	return ptr;
}

std::shared_ptr<ConfigParser::Token> ConfigParser::peekToken()
{
	return nextToken(false);
}

ConfigItem::ConfigItem(const std::string & name)
	: text_(name)
{

}

void ConfigItem::pushArgument(const std::string & arg)
{
	childItems_.emplace_back(std::make_shared<ConfigItem>(arg));
}

void ConfigItem::pushArgument(const std::shared_ptr<ConfigItem>& arg)
{
	childItems_.emplace_back(arg);
}

bool ConfigItem::item(const std::string & name, bool & value)
{
	std::string strValue;
	if (!item(name, strValue))
	{
		return false;
	}

	// get value
	if (strValue != "on" && strValue != "off")
	{
		LOG_ERROR << "ConfigItem::item error get " << name << " exception :" << "boolean value should be on or off.";
		return false;
	}

    value = strValue ==  "on" ? true : false;
	return true;
}

bool ConfigItem::item(const std::string & name, std::shared_ptr<ConfigItem>& configItem)
{
	// find first match
	for (auto item : childItems_)
	{
		if (item && name == item->text())
		{
			configItem = item;
			return true;
		}
	}
	return false;
}

bool ConfigItem::item(const std::string &name, std::vector<std::shared_ptr<ConfigItem>>& configItems)
{
	bool found = false;
	for (auto item : childItems_)
	{
		if (name == item->text())
		{
			configItems.push_back(item);
			found = true;
		}
	}
	return found;
}

bool ConfigItem::item(const std::string & name, int & value)
{
	std::string strValue;
	if (!item(name, strValue))
	{
		return false;
	}

	// get value
	try
	{
		value = std::stoi(strValue);
		return true;
	}
	catch (const std::exception &e)
	{
		LOG_ERROR << "ConfigItem::item error get " << name << " exception :" << e.what();
	}

	return false;
}

bool ConfigItem::item(const std::string & name, std::string & value)
{
	std::shared_ptr<ConfigItem> configItem;
	if (!item(name, configItem))
	{
		return false;
	}

	if (configItem = configItem->item())
	{
		// get value
		value = configItem->text();
		return true;
	}

	return false;
}
