/*
Simple RPN calculator for the arduino
Uses serial to recieve commands from PC and print output (9600 baud)
*/

#define INWORD 1
#define OUTWORD 0

char buffer[100];
char building[20];
int stack[100];
int point = 0;
int sp = -1;
char c;

void clearStr(char str[], int length){
  for(int i = 0; i < length; i++)
    str[i] = '\0';
}

void setup() {
  // initialize serial:
  Serial.begin(9600);
  Serial.println("Starting...");
  clearStr(buffer, 100);
}

bool isNumber(const char str[]){
  for(int i = 0; str[i] != '\0'; i++)
    if(!isDigit(str[i])) return false;
  return true;
}

int popStack(){
  int val = stack[sp--];
  return val;
}

bool checkStack(int items){
  if(sp + 1 >= items)
    return true;
  Serial.println("Stack underflow!");
  return false;
}

int charIndex(const char str[], char c){
  for(int i = 0; str[i] != '\0'; i++)
    if(str[i] == c) return i;
  return -1;
}

void performOperator(const char str[]){
  int left, right;
  int length = strlen(str);
  char c = str[0];
  if(strlen(str) > 1){
    if(strcmp(str, "dup") == 0){
      left = stack[sp];
      if(checkStack(1))
        stack[++sp] = left;
    } else if(strcmp(str, "swap") == 0){
      if(checkStack(2)){
        left = popStack();
        right = popStack();
        stack[++sp] = left;
        stack[++sp] = right;
      }
    } else if(strcmp(str, "over") == 0){
      left = stack[sp - 1];
      if(checkStack(2))
        stack[++sp] = left;
    } else {
      Serial.print("Invalid word: ");
      Serial.println(str);
    }
  }
  else if(charIndex("+-*/", c) > -1){
    right = popStack();
    left = popStack();
    switch(c){
      case '+': stack[++sp] = left + right; break;
      case '-': stack[++sp] = left - right; break;
      case '*': stack[++sp] = left * right; break;
      case '/': stack[++sp] = left / right; break;
    }
  } else {
    switch(c){
      case '.':
        if(checkStack(1))
          Serial.println(popStack());
        break;
      case 'p':
        for(int i = 0; i <= sp; i++){
          Serial.print(stack[i]);
          Serial.print(' ');
        }
        Serial.print('\n');
        break;
      case 'c':
        sp = -1;
        break;
      default:
        Serial.print("Inavlid word: ");
        Serial.println(c);
    }
  }
}

bool parseWord(const char str[]){
  if(isNumber(str)){
    stack[++sp] = atoi(str);
  } else {
    performOperator(str);
  }
  return true;
}

void parseBuffer(){
  extern char buffer[], building[];
  int count = 0;
  int state = 0;
  bool success = true;
  for(int i = 0; buffer[i] != '\0'; i++){
    clearStr(building, 20);
    count = 0;
    while(buffer[i] != ' ' && buffer[i] != '\0'){
      building[count] = buffer[i];
      count++;
      i++;
    }
    if(count > 0)
      if(!parseWord(building)) success = false;
  }
  return success;
}

void loop() {
  while (Serial.available() > 0) {
    c = Serial.read();
    if(c != '\n' && c != '\r'){
      buffer[point++] = c;
    }
    else {
      Serial.println(buffer);
      parseBuffer();
      clearStr(buffer, 100);
      point = 0;
    }
  }
}
