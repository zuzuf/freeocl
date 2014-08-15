/*
	FreeOCL - a free OpenCL implementation for CPU
	Copyright (C) 2011  Roland Brochard

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "preprocessor.h"
#include <utils/string.h>

namespace FreeOCL
{
	bool preprocessor::valid_chunk_for_macro_expansion(const std::string &text)
	{
		std::string word;
		for(size_t i = 0 ; i < text.size() ; ++i)
		{
			char c = text[i];
			if (isalpha(c) || c == '_')
			{
				word.clear();
				for(; i < text.size() && (isalnum(text[i]) || text[i] == '_') ; ++i)
					word += text[i];
				--i;
				if (macros.count(word))
				{
					const macro &m = macros[word];
					if (!m.params.empty())
					{
						++i;
						if (i >= text.size() || text[i] != '(')
						{
							--i;
							continue;
						}
						++i;
						int nb_args = 0;
						while(i < text.size() && text[i] != ')')
						{
							if (text[i] == ',')
								++i;
							while(i < text.size() && isspace(text[i]))	++i;
							int p_level = 0;
							bool b_in_string = false;
							bool b_escaped = false;
							bool b_empty = true;
							for(; i < text.size() && ((text[i] != ',' && text[i] != ')') || p_level > 0 || b_in_string) ; ++i)
							{
								if (text[i] == '"' && !b_escaped)
									b_in_string ^= true;
								b_escaped = (text[i] == '\\');
								if (!b_in_string)
								{
									if (text[i] == '(')
										++p_level;
									else if (text[i] == ')')
										--p_level;
								}
								if (!isspace(text[i]))
									b_empty = false;
							}

							if (!b_empty)
								nb_args++;
						}

						if (m.params.size() == 1 && m.params.front().empty())
						{
							if (nb_args > 0)
								return true;
						}
						else
						{
							if (nb_args != m.params.size())
								return false;
						}
					}
				}
			}
		}
		return true;
	}

    std::string preprocessor::macro_expansion(const std::string &text, const set<std::string> &processed_macros, const bool b_defined_rule)
	{
        std::string ret;
		std::string word;
		std::string value;
        bool b_last_was_defined = false;
        ret.reserve(text.size());
        for(size_t i = 0 ; i < text.size() ; ++i)
        {
            char c = text[i];
            if (isalpha(c) || c == '_')
            {
                word.clear();
                for(; i < text.size() && (isalnum(text[i]) || text[i] == '_') ; ++i)
                    word += text[i];
                --i;
				while(!word.empty())
					if (macros.count(word) && processed_macros.count(word) == 0
							&& (!b_last_was_defined || !b_defined_rule))
					{
						const macro &m = macros[word];
						set<std::string> already_expanded(processed_macros);
						already_expanded.insert(word);
						const size_t prev_ret_size = ret.size();
						if (m.params.empty())
							ret += macro_expansion(m.value, already_expanded);
						else
						{
							++i;
							for(; i < text.size() && isspace(text[i]) ; ++i)
								word += text[i];
							if (i >= text.size() || text[i] != '(')
							{
								--i;
								ret += word;
								word.clear();
								continue;
							}
							else
							{
								++i;
								std::vector<std::string> args;
								args.reserve(m.params.size());
								while(i < text.size() && text[i] != ')')
								{
									value.clear();
									if (text[i] == ',')
										++i;
									while(i < text.size() && isspace(text[i]))	++i;
									int p_level = 0;
									bool b_in_string = false;
									bool b_escaped = false;
									for(; i < text.size() && ((text[i] != ',' && text[i] != ')') || p_level > 0 || b_in_string) ; ++i)
									{
										if (text[i] == '"' && !b_escaped)
											b_in_string ^= true;
										b_escaped = (text[i] == '\\');
										if (!b_in_string)
										{
											if (text[i] == '(')
												++p_level;
											else if (text[i] == ')')
												--p_level;
										}
										value += text[i];
									}
									while(!value.empty() && isspace(*value.rbegin()))
										value.erase(value.size() - 1);

									args.push_back(value);
								}

								if (m.params.size() == 1 && m.params.front().empty())
								{
									if (args.size() > 0)
										error("wrong number of macro parameters (" + to_string(args.size()) + " but none expected)");
									ret += m.value;
								}
								else
								{
									if (args.size() != m.params.size())
										error("wrong number of macro parameters (" + to_string(args.size()) + " but " + to_string(m.params.size()) + " expected)");
									ret += macro_expansion(parameters_substitution(m.value, m.params, args), already_expanded);
								}
							}
						}

						const std::string orig_word = word;
						word.clear();
						if (!ret.empty())
						{
							int j;
							for(j = ret.size() - 1 ; j > prev_ret_size - 1; --j)
							{
								c = ret[j];
								if (isspace(c))
								{
									if (word.empty())
										continue;
									break;
								}
								if (isalnum(c) || c == '_')
									word = c + word;
								else
									break;
							}
							if (word == orig_word)
								word.clear();
							if (!word.empty())
								ret.resize(j + 1);
						}
					}
					else
					{
						ret += word;
						b_last_was_defined = (word == "defined");
						word.clear();
					}
            }
            else
                ret += c;
        }
        return ret;
	}

	std::string preprocessor::parameters_substitution(const std::string &s,
													  const std::vector<std::string> &names,
													  const std::vector<std::string> &values)
	{
		std::string ret;
		ret.reserve(s.size());
		std::string word;
		map<std::string, std::string> mapping;
		bool stringify = false, concat = false;
		for(size_t i = 0 ; i < names.size() ; ++i)
			mapping[names[i]] = values[i];

		for(size_t i = 0 ; i < s.size() ; ++i)
		{
			if(s[i] == '#')
			{
				if (!stringify)
					stringify = true;
				else
				{
					concat = true;
					stringify = false;
				}
				
			}
			else if (isalpha(s[i]) || s[i] == '_')
			{
				word.clear();
				for(; i < s.size() && (isalnum(s[i]) || s[i] == '_') ; ++i)
					word += s[i];
				--i;
				map<std::string, std::string>::const_iterator it = mapping.find(word);
				if (concat)
				{
					int c = ret.back();
					while(isspace(c))
					{
						ret.pop_back();
						c = ret.back();
					}
					concat = false;
				}
				if (it != mapping.end())
				{
					if (stringify)
					{
						ret += '"' + it->second + '"';
						stringify = false;
					}
					else
						ret += it->second;
				}
				else
				{
					if (stringify)
					{
						ret += '#' + word;
						stringify = false;
					}
					else
						ret += word;
				}
			}
			else
			{
				ret += s[i];
				stringify = false;
			}
		}
		return ret;
	}
}
