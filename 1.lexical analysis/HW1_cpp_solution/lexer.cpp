#include "lexer.h"

// Lexical error messages
const string INVALID_CHARCONST =
  "**** Invalid character constant";
const string INVALID_INTCONST =
  "**** Invalid integer constant";

// Static Lexer variables
bool Lexer::print_token_lexemes_flag = false;
bool Lexer::print_token_lexemes_token_flag = true;

Lexer::Lexer() {
	regular_expressions = {
		// --- Ignore ---
		make_pair(regex("^\\(\\*"), "start_comment"),
		make_pair(regex("^\\*\\)"), "end_comment"),
		make_pair(regex("^[ \\t]$"), "white_space"),
		// --------------
		make_pair(regex("^:=$"), "T_ASSIGN"),
		make_pair(regex("^\\*$"), "T_MULT"),
		make_pair(regex("^\\+$"), "T_PLUS"),
		make_pair(regex("^-$"), "T_MINUS"),
		make_pair(regex("^div$"), "T_DIV"),
		make_pair(regex("^and$"), "T_AND"),
		make_pair(regex("^or$"), "T_OR"),
		make_pair(regex("^not$"), "T_NOT"),
		make_pair(regex("^<$"), "T_LT"),
		make_pair(regex("^>$"), "T_GT"),
		make_pair(regex("^<=$"), "T_LE"),
		make_pair(regex("^>=$"), "T_GE"),
		make_pair(regex("^=$"), "T_EQ"),
		make_pair(regex("^<>$"), "T_NE"),
		make_pair(regex("^var$"), "T_VAR"),
		make_pair(regex("^array$"), "T_ARRAY"),
		make_pair(regex("^of$"), "T_OF"),
		make_pair(regex("^boolean$"), "T_BOOL"),
		make_pair(regex("^char$"), "T_CHAR"),
		make_pair(regex("^integer$"), "T_INT"),
		make_pair(regex("^program$"), "T_PROG"),
		make_pair(regex("^procedure$"), "T_PROC"),
		make_pair(regex("^begin$"), "T_BEGIN"),
		make_pair(regex("^end$"), "T_END"),
		make_pair(regex("^while$"), "T_WHILE"),
		make_pair(regex("^do$"), "T_DO"),
		make_pair(regex("^if$"), "T_IF"),
		make_pair(regex("^then$"), "T_THEN"),
		make_pair(regex("^else$"), "T_ELSE"),
		make_pair(regex("^read$"), "T_READ"),
		make_pair(regex("^write$"), "T_WRITE"),
		make_pair(regex("^true$"), "T_TRUE"),
		make_pair(regex("^false$"), "T_FALSE"),
		make_pair(regex("^\\[$"), "T_LBRACK"),
		make_pair(regex("^\\]$"), "T_RBRACK"),
		make_pair(regex("^;$"), "T_SCOLON"),
		make_pair(regex("^:$"), "T_COLON"),
		make_pair(regex("^\\($"), "T_LPAREN"),
		make_pair(regex("^\\)$"), "T_RPAREN"),
		make_pair(regex("^,$"), "T_COMMA"),
		make_pair(regex("^\\.$"), "T_DOT"),
		make_pair(regex("^\\.\\.$"), "T_DOTDOT"),
		make_pair(regex("^[0-9]+$"), "T_INTCONST"),
		make_pair(regex("^'.'$"), "T_CHARCONST"),
		make_pair(regex("^[a-zA-Z_][a-zA-Z_0-9]*$"),
                           "T_IDENT"),
		make_pair(regex("^''?$"), INVALID_CHARCONST),
		make_pair(regex("^.$"), "UNKNOWN")
	};
}

void Lexer::lexemes_to_tokens(vector<string> input) {
  bool in_comment = false;
  bool match_found = false;

  // Iterators to the part of the current word that we are
  // currently looking at
  string::iterator begin;
  string::iterator end;

  // Split input by newline
  for(auto& word : input) {
    // Set iterators to beginning and end of line		
    begin = word.begin();
    end = word.end();

    // While there are characters to be parsed in word
    while(begin != end) {
    // For each regex-token pair
    for(const auto& reg_str : regular_expressions) {
      // If there was a match
      if(regex_match(begin, end, reg_str.first)) {
        match_found = true;

        // Ignore whitespace
        if(reg_str.second == "white_space") {}
          // Invalid character constant error
        else if(reg_str.second == INVALID_CHARCONST) {
               if(print_token_lexemes_flag == true) {
		      cout << INVALID_CHARCONST << ": "
                      << get_lexeme_str(begin, end) << endl;
               }
		  }
        // Invalid integer constant error
	  else if(reg_str.second == "T_INTCONST" && 
               invalid_integer_constant(
                get_lexeme_str(begin, end))) {
		    if(print_token_lexemes_flag == true) {
                 cout << INVALID_INTCONST << ": "
                      << get_lexeme_str(begin, end) << endl;
               }
             }
       // If start of comment
	  else if(reg_str.second == "start_comment") {
              // Set in-comment context to true
              in_comment = true;
		  }
	  // If not in comment block, add token
	  else if(in_comment == false && 
               reg_str.second != "end_comment") {
		    add_token_lexeme(reg_str.second,
                                get_lexeme_str(begin, end));
             } 
	  // If "*)" found while not in comment block, split
	  else if(in_comment == false && 
               reg_str.second == "end_comment") {
		    // * 
		    add_token_lexeme("T_MULT", "*");
		    // )
		    add_token_lexeme("T_RPAREN", ")");
		  }
	  // If in a comment and end of comment lexeme found
	  else if(reg_str.second == "end_comment" && 
               in_comment == true) {
		    // Exist in-comment context
		    in_comment = false;
		  }

	// Increment beginning iterator to end
	begin = end;
	// Reset ending iterator
	end = word.end();

	break;
    }
  }

  if(match_found == false) {
    // Decrement greedy search
    end--;
  } else {
           match_found = false;
         }

  // If there is no match. This is only reached on window
  // machines.
  if(begin == end && begin != word.end()) {
    break;
  }
  }

  }
}

string Lexer::getToken() {
  string current_token;

  // If no more tokens to be processed
  if(getToken_iterator == tokens.end()) {
    // Return empty string
    return "";
  }

  // Get current token
  current_token = *getToken_iterator;

  // Increment token iterator
  getToken_iterator++;
  
  // print token
  print_token_lexemes(tokens.back(), lexemes.back());

  return current_token;
}

vector<string> Lexer::get_all_tokens() const {
  return tokens;
}

vector<string> Lexer::get_all_lexemes() const {
  return lexemes;
}

vector<string> Lexer::split(string const& input) const {
  // Temporary string
  string tmp = "";
  // Vector of each line
  vector<string> ret;

  // For each character
  for(const auto& c : input) {
  // If new line, add string and reset tmp string
    if(c == '\n') {
      ret.push_back(tmp);
	 tmp = "";
    } else { // Else add character to tmp string
			tmp += c;
    }
  }

  // Add last string if one exists
  if(tmp != "") {
    ret.push_back(tmp);
  }

  return ret;
}

bool Lexer::invalid_integer_constant(const string& lexeme) const {
  const string max_int = "2147483647";

  string number = lexeme;

  // Removing leading zeros
  number.erase(0, min(number.find_first_not_of('0'),
                      number.size()-1));

  // If number is less than 10 digits, good
  if(number.size() < 10) {
		return false;
  } 
  // If number is greater than 10 digits, bad
  else if(number.size() > 10) {
		return true;
       } 
       // If number is exactly 10 digits, check numbers
	 else {
		// For each digit in number and max_int
		for(unsigned int i = 0; i < number.size(); i++) {
		  // If number char is greater than 
             // max_int char, bad
		  if(number[i] > max_int[i]) {
		    return true;
		  }
		}
	}
	return false;
}

void Lexer::add_token_lexeme(const string& token, 
                             const string& lexeme) {
  // Add token and lexeme
  tokens.push_back(token);
  lexemes.push_back(lexeme);

  // If print flag is set, print token and lexeme
  if(print_token_lexemes_flag == true) {
    cout << "TOKEN: " << tokens.back() << "\t" << setw(12)
         << "LEXEME: " << lexemes.back() << endl;
  }

  // Reset getToken_iterator in-case vector got resized
  getToken_iterator = tokens.begin();
}


string Lexer::get_lexeme_str(string::iterator begin,
                             string::iterator end) const {
  string lexeme = "";
  for (auto it = begin; it < end; it++) {
    lexeme += *it;
  }

  return lexeme;
}

void Lexer::print_token_lexemes(const string& token,
                                const string& lexeme)
{
    // If print flag is set, print token and lexeme
  if(print_token_lexemes_token_flag == true) {
    cout << "TOKEN: " << token << "\t" << setw(12)
         << "LEXEME: " << lexeme << endl;
  }
}
                                    
                                    