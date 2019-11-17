////////////////////////////////////////////////////////////
/// @file	lexer.h
/// @brief 	Tokenizes a given input
////////////////////////////////////////////////////////////

#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <vector>
#include <utility>
#include <regex>
#include <iomanip>
using namespace std;

extern ostringstream oal_prog;

class Lexer 
{
  public:
		////////////////////////////////////////////////////////////
		/// @brief	Initializes the Lexer class
		/// @post 	regular_expressions will be populated with 
		/// the correct mapping between regex and token.
		////////////////////////////////////////////////////////////
		Lexer();

		////////////////////////////////////////////////////////////
		/// @brief	Tokenizes given string input
		/// @param[in]	input 	Input to tokenize
		/// @post 	tokens and lexeme is populated based on
		/// input
		////////////////////////////////////////////////////////////
		void lexemes_to_tokens(vector<string> input);

		////////////////////////////////////////////////////////////
		/// @brief	Get the next token to be processed along
		///        with its 
		/// 		corresponding lexeme and line number.
		/// 		If print_token_lexemes_flag is set, print 
		///        token and lexeme
		/// 		in the following format:
		/// 		TOKEN: <TOKEN>      LEXEME: <LEXEME>
		/// @post 	getToken_index is incremented by 1
		/// @return The next token to be processed along with 
		/// 		 its corresponding lexeme and line number. 
		///		 If all tokens have been processed, then
		///         empty string ("").
		/// 		 Order:
		///		 ["Token", "Lexeme", "Line Number"]
		////////////////////////////////////////////////////////////
		vector<string> getToken();

		////////////////////////////////////////////////////////////
		/// @brief	Get all tokens
		/// @return 	All tokens in tokens. Tokens and 
		/// 			lexemes will be in the same order.
		////////////////////////////////////////////////////////////
		vector<string> get_all_tokens() const;

		////////////////////////////////////////////////////////////
		/// @brief	Get all lexemes
		/// @return 	All lexemes in lexemes. Tokens and 
		/// 			lexemes will be in the same order.
		////////////////////////////////////////////////////////////
		vector<string> get_all_lexemes() const;

		static bool print_token_lexemes_flag; 
		///< Print Tokens, Lexemes, and
		///< errors while reading in data
	private:
		////////////////////////////////////////////////////////////
		/// @brief	Splits the given input by newline
		/// @param[in] input	Input to be split
		/// @return 	Vector of strings with no newline
		////////////////////////////////////////////////////////////
		vector<string> split(string const& input) const; 


		////////////////////////////////////////////////////////////
		/// @brief	Checks to see is lexeme is an invalid
		/// integer constant which is defined as a number 
		/// greater than "2147483647"
		/// @returns true if invalid integer constant, false
		/// otherwise
		////////////////////////////////////////////////////////////
		bool invalid_integer_constant(const string& lexeme)
           const;

		////////////////////////////////////////////////////////////
		/// @brief	Adds token and lexeme to their respective 
		/// vectors
		/// @param[in] token 	Token to add to tokens
		/// @param[in] lexeme 	Lexeme to add to lexemes
		/// @param[in] line_number 	The line number the lexeme
		/// appeared on
		/// @post 	token and lexeme are added to their
		/// respective vectors
		////////////////////////////////////////////////////////////
		void add_token_lexeme(const string& token, 
                                 const string& lexeme, 
			                 const int line_number);

		////////////////////////////////////////////////////////////
		/// @brief	Converts iterator to string
		/// @param[in]	begin 	Start iterator of string
		/// @param[in] 	end 	End iterator of string
		/// @return 	String represented by begin to end
		////////////////////////////////////////////////////////////
		string get_lexeme_str(string::iterator begin,
                                 string::iterator end) const;

		vector<pair<regex, string> > regular_expressions; 				///< Mapping between
			///< the regular expressions and the 
			///< tokens they represent.
			///< The order in which this is populated 
			///< is the order in which
			///< like lexemes will be tokenized

		vector<string> tokens;	
		///< The tokens derived from the inputed lexemes
		vector<string> lexemes;	
		///< The lexemes that are from the given input

		vector<string> linenumber; 
		///< Line number of token/lexeme

		int getToken_index; 
		///< Index to keep track of the next token 
		///< to give back for getToken()

};

#endif
