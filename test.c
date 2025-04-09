# include <stdio.h>
# include <stdbool.h>
# include <stdlib.h>
# include <string.h>

# define NOT !
# define AND &&
# define OR ||
# define EQ ==

enum TokenType {
  IDENTIFIER = 34313,
  CONSTANT = 87424,
  SPECIAL = 29535
};

typedef char * CharPtr;

typedef struct TokenInfo {
  CharPtr token_str;
  enum TokenType token_type;
  int first_char_line;
  int first_char_column;
}TokenInfo;

typedef TokenInfo * TokenInfoPtr;

void GetNextChar(char *ch, int *line, int *column);
bool IsDigit(char ch);
bool IsLetter(char ch);
bool IsWhiteSpace(char ch);
bool IsSpecial(char ch);
void SkipWhiteSpaces(char *ch, int *line, int *column);
void SkipLineComment(char *ch, int *line, int *column);
void SkipMultiLineComment(char *ch, int *line, int *column);
CharPtr GetID(char *ch, int *line, int *column);
CharPtr GetNum(char *ch, int *line, int *column);
CharPtr GetCharConst(char *ch, int *line, int *column);
CharPtr GetStringConst(char *ch, int *line, int *column);
CharPtr GetSpecial(char *ch, int *line, int *column);
TokenInfoPtr GetToken(char *ch, int *current_line, int *current_column);

typedef struct Column {
  int column;
  struct Column * next;
}Column;

typedef Column * ColumnPtr;

typedef struct Line {
  int line;
  ColumnPtr first_appear_at;
  ColumnPtr last_appear_at;
  struct Line * next;
}Line;

typedef Line * LinePtr;

typedef struct Token {
  CharPtr token_str;
  enum TokenType type;
  LinePtr first_appear_on;
  LinePtr last_appear_on;
  struct Token * next;
}Token;

typedef Token * TokenPtr;

void PrintAllTokenInfo(TokenPtr head);
TokenPtr OrderInsertToken(TokenPtr head, TokenInfoPtr info);
void CountTokens(TokenPtr head);
void CountTypes(TokenPtr head);
void FindTokenAppearance(TokenPtr head, char *str);
void ListTokenAtLine(TokenPtr head, int line_no);
void HandleUI(TokenPtr head);

int g_next_char_line = 1;  // line-no of the next char
int g_next_char_column = 1;  // column-no of the next char

int main() {

  char ch = '\0';
  int current_line = -1;   // line-no of current char
  int current_column = -1;   // column-no of current char
  TokenInfoPtr info_ptr = NULL;
  TokenPtr list_head = NULL;

  GetNextChar(&ch, &current_line, &current_column);  // get the very first char

  do {
    info_ptr = GetToken(&ch, &current_line, &current_column);

    if ( info_ptr != NULL ) {
      if (strcmp(info_ptr->token_str, "END_OF_EVERYTHING") == 0) {
        free(info_ptr);
        info_ptr = NULL;
      } else
        list_head = OrderInsertToken(list_head, info_ptr);
    }
  } while ( info_ptr != NULL );

  HandleUI(list_head);

  return 0;
}
void CountTokens(TokenPtr head) {
  TokenPtr current = head;
  int count = 0;
  while (current != NULL) {
    count++;
    current = current->next;
  }
  printf("There are %d different tokens in total.\n", count);

}
void CountTypes(TokenPtr head) {
  TokenPtr current = head;
  int count_ide = 0;
  int count_cons = 0;
  int count_spe = 0;
  while (current != NULL) {
    if (current->type == IDENTIFIER) {
      count_ide++;
    }

    else if (current->type == CONSTANT) {
      count_cons++;
    }

    else if (current->type == SPECIAL) {
      count_spe++;
    }
    current = current->next;
  }
  printf("Identifer : %d\n", count_ide);
  printf("Constant : %d\n", count_cons);
  printf("Special : %d\n", count_spe);
}
void FindTokenAppearance(TokenPtr head, char *str) {
  TokenPtr current = head;
  while (current != NULL) {
    if (strcmp(str, current->token_str) == 0) {
      LinePtr line = current->first_appear_on;
      while (line != NULL) {
        printf("(%d,%d)", line->line, line->first_appear_at->column);
        line = line->next;
      }
      printf("\n");
      return;
    }
    current = current->next;
  }
  if (current == NULL) {
    char ans[1000] = "";
    strcpy(ans, str);
    printf("There is no such token : %s\n", ans);
    return;
  }

}
void ListTokenAtLine(TokenPtr head, int line_no) {
  TokenPtr current = head;
  bool found = false;
  while (current != NULL) {
    LinePtr line = current->first_appear_on;
    while (line != NULL) {
      if (line->line == line_no) {
        found = true;
        char ans[1000] = "";
        strcpy(ans, current->token_str);
        printf("%s\n", ans);
      }
      line = line->next;
    }
    current = current->next;
  }
  if (found) {
    return;
  }
  if (current == NULL) {
    printf("There is no such line : %d\n", line_no);
    return;
  }
}
void HandleUI(TokenPtr head) {
  int action = 0;
  while (scanf("%d", &action) != EOF) {
    if (action == 1) {
      CountTokens(head);
    }

    else if (action == 2) {
      CountTypes(head);
    }

    else if (action == 3) {
      char c;
      scanf("%c", &c);
      TokenInfoPtr token = NULL;
      int now_line = -1;
      int now_column = -1;
      token = GetToken(&c, &now_line, &now_column);
      FindTokenAppearance(head, token->token_str);
    }

    else if (action == 4) {
      int num = 0;
      scanf("%d", &num);
      ListTokenAtLine(head, num);
    }

    else if (action == 5) {
      PrintAllTokenInfo(head);
    }

    else if (action == 6) {
      break;
    }
  }
  return;
}
void PrintAllTokenInfo(TokenPtr head) {
  if (head == NULL) {
    return;
  }
  printf("%s ",  head->token_str);
  LinePtr line = head->first_appear_on;
  while (line != NULL) {
    printf("(%d,%d)", line->line, line->first_appear_at->column);
    line = line -> next;
  }
  printf("\n");
  PrintAllTokenInfo(head->next);
}
TokenPtr OrderInsertToken(TokenPtr head, TokenInfoPtr info) {
  if (head == NULL || (strcmp(info->token_str, head->token_str) < 0)) {
    TokenPtr neww = malloc(sizeof(Token));
    int len = strlen(info->token_str) + 1;
    neww->token_str = malloc(sizeof(char) * len);
    strcpy(neww->token_str, info->token_str);

    neww->type = info->token_type;

    LinePtr new_line = malloc(sizeof(Line));
    ColumnPtr new_col = malloc(sizeof(Column));

    new_line->line = info->first_char_line;
    new_col->column = info->first_char_column;
    new_line->next = NULL;
    new_col->next = NULL;

    new_line->first_appear_at = new_col;
    new_line->last_appear_at = new_col;

    neww->first_appear_on = new_line;
    neww->last_appear_on = new_line;

    neww->next = head;
    return neww;
  }

  if (strcmp(info->token_str, head->token_str) == 0) {
    LinePtr new_line = malloc(sizeof(Line));
    ColumnPtr new_col = malloc(sizeof(Column));

    new_line->line = info->first_char_line;
    new_col->column = info->first_char_column;
    new_line->next = NULL;
    new_col->next = NULL;

    new_line->first_appear_at = new_col;
    new_line->last_appear_at = new_col;

    head->last_appear_on->next = new_line;
    head->last_appear_on = new_line;
    return head;
  }

  head->next = OrderInsertToken(head->next, info);
  return head;
}
TokenInfoPtr GetToken(char *ch, int *current_line, int *current_column) {
  TokenInfoPtr answer = malloc(sizeof(TokenInfo));
  bool slash_is_divide = false;
  int line_of_divide = -1, column_of_divide = -1;
  CharPtr token_str = NULL;
  while (1) {
    if (*ch == EOF)
      break;
    slash_is_divide = false;  // assume false for the moment
    // skip white spaces and comments
    // (including line comments and multi-line comments)
    // '/' may be the DIVIDE operand)

    while (IsWhiteSpace(*ch) OR(*ch == '/' AND NOT slash_is_divide)) {

      if ( IsWhiteSpace(*ch) )
        SkipWhiteSpaces(ch, current_line, current_column);

      else if (*ch == '/') {

        // Be careful!!! '/' may be DIVIDE

        line_of_divide = g_next_char_line;
        column_of_divide = g_next_char_column;

        // need to know what is behind '/'
        GetNextChar(ch, current_line, current_column);

        if (*ch == '/' )
          SkipLineComment(ch, current_line, current_column);

        else if (*ch == '*' )
          SkipMultiLineComment(ch, current_line, current_column);

        else {  // the previous '/' IS DIVIDE

          slash_is_divide = true;
          // the char before current ch is DIVIDE operand
          // do special handling below for this DIVIDE
          // because ch may be '=', i.e. "/="
        }
      }
    }

    // now ch is non-white-space and ch may be the char behind DIVIDE
    // depends on slashIsDIVIDE
    // line, column of the first char of this token
    int line = *current_line, column = *current_column;

    answer->first_char_line = *current_line;
    answer->first_char_column = *current_column;

    if ( slash_is_divide ) {

      // Instead of calling GetSpecial(),
      // we need to do special handling of DIVIDE here
      if (*ch == '=') {  // the char behind '/' is '=' ; i.e., "/="
        char input[5] = "";
        input[0] = '/';
        input[1] = '=';
        answer->first_char_column -= 1;
        int len = strlen(input) + 1;
        answer->token_str = malloc(sizeof(char) * len);
        strcpy(answer->token_str, input);
        answer->token_type = SPECIAL;
        GetNextChar(ch, current_line, current_column);
        return answer;
      }
      // ch is not '=', therefore, the previous '/' is a one-char-DIVIDE
      else {
        char input[5] = "";
        input[0] = '/';
        answer->first_char_column -= 2;
        int len = strlen(input) + 1;
        answer->token_str = malloc(sizeof(char) * len);
        strcpy(answer->token_str, input);
        answer->token_type = SPECIAL;
        return answer;
        // no need to cal  GetNextChar() again
        // ch is already the next char behind DIVIDE
      }
    }

    else if ( IsLetter(*ch) || *ch == '_' ) {
      token_str = GetID(ch, current_line, current_column);
      int len = strlen(token_str) + 1;
      answer->token_str = malloc(sizeof(char) * len);
      strcpy(answer->token_str, token_str);
      answer->token_type = IDENTIFIER;
      return answer;
    }

    else if ( IsDigit(*ch) || *ch == '.' ) {
      token_str = GetNum(ch, current_line, current_column);
      int len = strlen(token_str) + 1;
      answer->token_str = malloc(sizeof(char) * len);
      strcpy(answer->token_str, token_str);
      answer->token_type = CONSTANT;
      return answer;
    }

    else if (*ch == '\'') {
      token_str = GetCharConst(ch, current_line, current_column);
      int len = strlen(token_str) + 1;
      answer->token_str = malloc(sizeof(char) * len);
      strcpy(answer->token_str, token_str);
      answer->token_type = CONSTANT;
      return answer;
    }

    else if (*ch == '\"') {
      token_str = GetStringConst(ch, current_line, current_column);
      int len = strlen(token_str) + 1;
      answer->token_str = malloc(sizeof(char) * len);
      strcpy(answer->token_str, token_str);
      answer->token_type = CONSTANT;
      return answer;
    }

    else if ( IsSpecial(*ch) ) {
      token_str = GetSpecial(ch, current_line, current_column);
      int len = strlen(token_str) + 1;
      answer->token_str = malloc(sizeof(char) * len);
      strcpy(answer->token_str, token_str);
      answer->token_type = SPECIAL;
      return answer;
    }

    else if (*ch != EOF) {  // Unrecognized token!!!
      GetNextChar(ch, current_line, current_column);
    }
  }

  if (token_str == NULL) {
    return NULL;
  }
}

void GetNextChar(char *ch, int *line, int *column) {
  *line = g_next_char_line;
  *column = g_next_char_column;
  if (scanf("%c", ch) == EOF) {
    *ch = EOF;
    return;
  }
  g_next_char_column = g_next_char_column + 1;
  if (*ch == '\n') {
    g_next_char_line = g_next_char_line + 1;
    g_next_char_column = 1;
  }
}

bool IsDigit(char ch) {
  if (ch <= '9' && ch >= '0') {
    return true;
  }

  else {
    return false;
  }
}

bool IsLetter(char ch) {
  if (ch <= 'z' && ch >= 'a') {
    return true;
  }

  else if (ch <= 'Z' && ch >= 'A') {
    return true;
  }

  else {
    return false;
  }
}

bool IsWhiteSpace(char ch) {
  if (ch == ' ' || ch == '\t' || ch == '\n') {
    return true;
  }

  else {
    return false;
  }
}

bool IsSpecial(char ch) {
  if (ch == '^' || ch == ',' || ch == '(' || ch == ')' || ch == '[' ||
      ch == ']' || ch == '{' || ch == '}' || ch == '!' || ch == ':' ||
      ch == ';' || ch == '#' || ch == '?' || ch == '+' || ch == '-' ||
      ch == '*' || ch == '/' || ch == '>' || ch == '<' || ch == '=' ||
      ch == '%' || ch == '&' || ch == '|') {
    return true;
  }

  else {
    return false;
  }
}

void SkipWhiteSpaces(char *ch, int *line, int *column) {
  while (IsWhiteSpace(*ch)) {
    GetNextChar(ch, line, column);
  }
}

void SkipLineComment(char *ch, int *line, int *column) {
  GetNextChar(ch, line, column);

  while (*ch != EOF && *ch != '\n') {
    GetNextChar(ch, line, column);
  }

  if (*ch == '\n') {
    GetNextChar(ch, line, column);
  }
}

void SkipMultiLineComment(char *ch, int *line, int *column) {
  GetNextChar(ch, line, column);
  while (*ch != EOF) {
    if (*ch == '*') {
      GetNextChar(ch, line, column);
      if (*ch == '/') {
        GetNextChar(ch, line, column);
        return;
      }
    }

    else {
      GetNextChar(ch, line, column);
    }
  }
}

CharPtr GetID(char *ch, int *line, int *column) {
  char input[100] = "";
  input[0] = *ch;
  GetNextChar(ch, line, column);
  int i = 1;
  while (IsLetter(*ch) || IsDigit(*ch) || *ch == '_') {
    input[i] = *ch;
    i++;
    GetNextChar(ch, line, column);
  }
  CharPtr answer1 = malloc(sizeof(char) * (i + 1));
  strcpy(answer1, input);
  return answer1;
}

CharPtr GetNum(char *ch, int *line, int *column) {
  char input[100] = "";
  input[0] = *ch;
  int i = 1;
  GetNextChar(ch, line, column);

  while (IsDigit(*ch) || *ch == '.') {
    input[i] = *ch;
    i++;
    GetNextChar(ch, line, column);
  }
  CharPtr answer2 = malloc(sizeof(char) * (i + 1));
  strcpy(answer2, input);

  return answer2;
}

CharPtr GetCharConst(char *ch, int *line, int *column) {
  char input[100] = "";
  input[0] = *ch;
  GetNextChar(ch, line, column);
  int i = 1;
  while (*ch != EOF) {
    if (*ch == '\\') {
      input[i] = *ch;
      i++;
      GetNextChar(ch, line, column);
      input[i] = *ch;
      i++;
      GetNextChar(ch, line, column);
    }

    else if (*ch == '\'') {
      input[i] = *ch;
      i++;
      GetNextChar(ch, line, column);
      break;
    }

    else {
      input[i] = *ch;
      i++;
      GetNextChar(ch, line, column);
    }
  }
  CharPtr answer3 = malloc(sizeof(char) *  (i + 1));
  strcpy(answer3, input);
  return answer3;
}

CharPtr GetStringConst(char *ch, int *line, int *column) {
  char input[1000] = "";
  input[0] = *ch;
  GetNextChar(ch, line, column);

  int i = 1;

  while (*ch != EOF) {
    if (*ch == '\\') {
      input[i] = *ch;
      i++;
      GetNextChar(ch, line, column);
      input[i] = *ch;
      i++;
      GetNextChar(ch, line, column);
    }

    else if (*ch == '\"') {
      input[i] = *ch;
      i++;
      GetNextChar(ch, line, column);
      break;
    }

    else {
      input[i] = *ch;
      i++;
      GetNextChar(ch, line, column);
    }
  }
  input[i] = '\0';
  int len = i + 1;
  CharPtr answer3 = malloc(sizeof(char) * len);
  strcpy(answer3, input);
  return answer3;
}

CharPtr GetSpecial(char *ch, int *line, int *column) {
  char input[5];
  int i = 0;
  char first = *ch;

  input[i] = first;
  i++;
  GetNextChar(ch, line, column);

  if (IsSpecial(*ch) &&
      ((first == '+' && (*ch == '+' || *ch == '=')) ||
       (first == '-' && (*ch == '-' || *ch == '=' || *ch == '>')) ||
       (first == '*' && *ch == '=') || (first == '/' && *ch == '=') ||
       (first == '>' && (*ch == '>' || *ch == '=')) ||
       (first == '<' && (*ch == '<' || *ch == '=')) ||
       (first == '=' && *ch == '=') || (first == '!' && *ch == '=') ||
       (first == '%' && *ch == '=') || (first == '&' && *ch == '&') ||
       (first == '|' && *ch == '|'))) {

    input[i] = *ch;
    i++;
    GetNextChar(ch, line, column);
  }

  input[i] = '\0';

  CharPtr answer = malloc(sizeof(char) * (i + 1));
  strcpy(answer, input);
  return answer;
}
