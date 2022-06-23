#pragma once

#include "std.h"

class ConfigItem
{
public:
	ConfigItem(const std::string &name);

	void pushArgument(const std::string &arg);
	void pushArgument(const std::shared_ptr<ConfigItem> &arg);

	// first matching item
	bool item(const std::string &name, std::shared_ptr<ConfigItem>& configItem);
	// all matching items
	bool item(const std::string &name, std::vector<std::shared_ptr<ConfigItem>>& configItems);
	bool item(const std::string &name, bool &value);
	bool item(const std::string &name, int &value);
	bool item(const std::string &name, std::string &value);
	std::shared_ptr<ConfigItem> item(int pos = 0) 
	{ 
		if (pos < size())
			return childItems_[pos];
		return nullptr;
	}

	const std::string &text() const { return text_; }
	const std::size_t size() const { return childItems_.size(); }
private:
	std::string text_;
	std::vector<std::shared_ptr<ConfigItem>> childItems_;
};

class ConfigParser
{
public:
	ConfigParser(const std::string &con)
		: content_(con)
	{

	}

	std::shared_ptr<ConfigItem> parse();
	std::shared_ptr<ConfigItem> parseConfigItem();

private:
	class Token
	{
	public:
		enum Type
		{
			Sign,
			Comment,
			Symbol,
		};

		Token(Type type, const std::string &content)
			: type_(type)
			, content_(content)
		{

		}
		const std::string& content() const { return content_; }
		bool is(Type type) const { return type_ == type; }
		bool is(Type type, const std::string &content) const { return is(type) && content_ == content; }
	private:
		std::string content_;
		Type type_;
	};

private:
	bool tokenize();
	bool next(char &ch, bool adv = true);
	bool peek(char &ch);

	std::shared_ptr<Token> nextToken(bool adv = true);
	std::shared_ptr<Token> peekToken();
private:
	int lineNumber_ = 0;
	int offset_ = 0;
	int tokenOffset_ = 0;
	std::string content_;
	std::vector<std::shared_ptr<Token>> tokens_;
};


