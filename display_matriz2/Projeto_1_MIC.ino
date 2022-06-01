/*

Gustavo H. S. Zanetti

Projeto 1 MICROCONTROLADORES 

DISPLAY DESLIZANTE PROGRAMÁVEL

*/

int vel = 20; //número de vezes que o frame será repetido
int pauseDelay = 1000;  //microsecondos para deixar cada linha acesa antes de mover para o próximo

String str;
char requestString[20];


//Variáveis usadas para rolagem ambas começam em 0
int index = 0;  //este é o caracter na string sendo mostrada
int offset = 0; //esta são quantas colunas de offset
 
//definição de pinos
int linhaA[] = {7,8,9,10,11,12};  //matriz que define quais são os pinos de cada linha no Arduino
                                        //(linhas são anodo comum (acionada com HIGH))
int colA[] = {2,3,4,5,6};      //matriz que define quais são os pinos de cada coluna no Arduino
                                        //(colunas são catodo comum (acionada com LOW))
 
//constantes definindo cada posição de caracter numa matriz de inteiros
//Letras
const int A = 0;  const int B = 1;  const int C = 2;  const int D = 3;  const int E = 4;
const int F = 5;  const int G = 6;  const int H = 7;  const int I = 8;  const int J = 9;
const int K = 10; const int L =11;  const int M = 12; const int N = 13; const int O = 14;
const int P = 15; const int Q =16;  const int R = 17; const int S = 18; const int T = 19;
const int U = 20; const int V =21;  const int W = 22; const int X = 23; const int Y = 24;
const int Z = 25;
 
 
//Matriz usada para armazenar um mapa de bits a ser mostrado(se você desejar fazer algum outro modifique os dados desta variável)
byte data[] = {0,0,0,0,0,0};
 
//O alfabeto
//cada caracter é um mapa de bit de 8 x 7 onde 1 é ligado e 0 é desligado
const int _A[] = {B00100,
                  B01010,
                  B10001,
                  B11111,
                  B10001,
                 };
 
const int _B[] = {B11111,
                  B10001,
                  B11110,
                  B10001,
                  B11111,
                  
                 };
 
const int _C[] = {B01111,
                  B10000,
                  B10000,
                  B10000,
                  B01111,
                 };
const int _D[] = {B11110,
                  B10001,
                  B10001,
                  B10001,
                  B11110,
                 };
 
const int _E[] = {B11111,
                  B10000,
                  B11110,
                  B10000,
                  B11111,
                 };
 
const int _F[] = {B11111,
                  B10000,
                  B11110,
                  B10000,
                  B10000,
                 };
 
const int _G[] = {B11111,
                  B10000,
                  B10011,
                  B10001,
                  B11111,
                 };
 
const int _H[] = {B10001,
                  B10001,
                  B11111,
                  B10001,
                  B10001,
                 };
 
const int _I[] = {B11111,
                  B00100,
                  B00100,
                  B00100,
                  B11111,
                 };
 
const int _J[] = {B11111,
                  B00010,
                  B00010,
                  B10010,
                  B11100,
                 };
 
const int _K[] = {B10010,
                  B10100,
                  B11000,
                  B10100,
                  B10010,
                 };
 
const int _L[] = {B10000,
                  B10000,
                  B10000,
                  B10000,
                  B11111,
                 };
 
const int _M[] = {B10001,
                  B11011,
                  B10101,
                  B10001,
                  B10001,
                 };
 
const int _N[] = {B10001,
                  B11001,
                  B10101,
                  B10011,
                  B10001,
                 };
 
const int _O[] = {B01110,
                  B10001,
                  B10001,
                  B10001,
                  B01110,
                 };
 
const int _P[] = {B11111,
                  B10001,
                  B11111,
                  B10000,
                  B10000,
                 };
 
const int _Q[] = {B11111,
                  B10001,
                  B11111,
                  B00001,
                  B00001,
                 };
 
const int _R[] = {B11111,
                  B10001,
                  B11111,
                  B10100,
                  B10010,
                 };
 
const int _S[] = {B11111,
                  B10000,
                  B11111,
                  B00001,
                  B11111,
                 };
 
const int _T[] = {B11111,
                  B00100,
                  B00100,
                  B00100,
                  B00100,
                 };
 
const int _U[] = {B10001,
                  B10001,
                  B10001,
                  B10001,
                  B11111,
                 };
 
const int _V[] = {B00000,
                  B10001,
                  B10001,
                  B01110,
                  B00100,
                 };
 
const int _W[] = {B10001,
                  B10001,
                  B10001,
                  B01010,
                  B00100,
                 };
 
const int _X[] = {B10001,
                  B01010,
                  B00100,
                  B01010,
                  B10001,
                 };
 
const int _Y[] = {B10001,
                  B01010,
                  B00100,
                  B01000,
                  B10000,
                 };
 
const int _Z[] = {B11111,
                  B00010,
                  B00100,
                  B01000,
                  B11111,
                 };

 
//carrega o mapa de bits de caracter numa matriz cada posição de caracter corresponde ao seu indice previamente definido, isto é _A (mapa de bits do "a")se o indice 0 é do "A", então letters[A] retornará o mapa de bits de "A")
const int* letters[] = {_A,_B,_C,_D,_E,_F,_G,_H,_I,_J,_K,_L,_M,_N,_O,_P,_Q,_R,_S,_T,_U,_V,_W,_X,_Y,_Z};
 
//definição do Arduino roda somente na inicialização
void setup()
{
  Serial.begin(9600);
  for(int i = 0; i <6; i++){  //configura os 16 pinos usados para controlar a matriz como pinos de saída
    pinMode(linhaA[i], OUTPUT);
    pinMode(colA[i], OUTPUT);
  }
}
 
//loop do programa principal
void loop(){
   while(Serial.available()){
    str=Serial.readString();
    str.toCharArray(requestString,20);
    Serial.println(requestString);
   }
   updateMatrix(); //atualiza a matrix com informação a ser mostrada
}
 
void updateMatrix(){
  loadSprite(); //carrega mapa de bits
  showSprite(vel); //mostra o mapa com a velocidade programada
}
 
//uma matriz contendo a potência de 2 usada como máscara de bits para calcular o que mostrar
const int powers[] = {1,2,4,8,16,32,64,128};
 
//carrega o estado atual do quadro a ser mostrado na matriz data[]
void loadSprite(){
  int currentChar = getChar(requestString[index]); //lê caracter da matriz
  int proximaVar = getChar(requestString[index+1]); //lê o próximo
 
  for(int linha=0; linha < 6; linha++){              //iteração para cada linha
    data[linha] = 0;                                 //reseta a linha de trabalho
    for(int coluna=0; coluna < 5; coluna++){         //iteração para cada coluna
     //carrega o caractere atual, compensado por pixels de deslocamento
     data[linha] = data[linha] + ((powers[coluna] & (letters[currentChar][linha] << offset)));
     //carrega o próximo caracte compensado por pixels de deslocamento
     data[linha] = data[linha] + (powers[coluna] & (letters[proximaVar][linha] >> (5-offset) ));
    }
  }
  offset++; //incrementa o offset de uma linha
  //se offset é de 5, carregar o par de caracteres a seguir na proxima vez
  if(offset==5){offset = 0; index++; if(index==sizeof(requestString)-2){index=0;}}
}
 
void showSprite(int vel2){
 for(int iii = 0; iii < vel2; iii++){                 //mostra o frame de vel2
  for(int coluna = 0; coluna < 5; coluna++){          //iteração de cada coluna
   for(int i = 0; i < 6; i++){
       digitalWrite(linhaA[i], LOW);                  //desliga todos os pinos de linhas
   }
   for(int i = 0; i < 5; i++){ //configura somente um pino
     if(i == coluna){     digitalWrite(colA[i], LOW);}  //liga a linha corrente
     else{                digitalWrite(colA[i], HIGH); }//liga o restante das linhas
   }
 
   for(int linha = 0; linha < 6; linha++){ //iteração através de cada pixel da coluna corrente
    int bit = (data[coluna] >> linha) & 1;
    if(bit == 1){
      //se o bit na matriz de dados está definido, acende o LED
      digitalWrite(linhaA[linha], HIGH);
    }
 
   }
   //deixar a coluna em pauseDelay microsecondos (muito alta a demora causa tremulação)
   delayMicroseconds(pauseDelay);
  }
 }
}
 
//retorna o indice de um dado caractere para a conversão de uma string e para a pesquisa na matriz de mapa de bit de caracteres
int getChar(char charachter){
 int returnValue = _; //Retorna Z toda vez que não reconhece o caracter
 switch(charachter){
  case 'A': returnValue = A; break;
  case 'a': returnValue = A; break;
  case 'B': returnValue = B; break;
  case 'b': returnValue = B; break;
  case 'C': returnValue = C; break;
  case 'c': returnValue = C; break;
  case 'D': returnValue = D; break;
  case 'd': returnValue = D; break;
  case 'E': returnValue = E; break;
  case 'e': returnValue = E; break;
  case 'F': returnValue = F; break;
  case 'f': returnValue = F; break;
  case 'G': returnValue = G; break;
  case 'g': returnValue = G; break;
  case 'H': returnValue = H; break;
  case 'h': returnValue = H; break;
  case 'I': returnValue = I; break;
  case 'i': returnValue = I; break;
  case 'J': returnValue = J; break;
  case 'j': returnValue = J; break;
  case 'K': returnValue = K; break;
  case 'k': returnValue = K; break;
  case 'L': returnValue = L; break;
  case 'l': returnValue = L; break;
  case 'M': returnValue = M; break;
  case 'm': returnValue = M; break;
  case 'N': returnValue = N; break;
  case 'n': returnValue = N; break;
  case 'O': returnValue = O; break;
  case 'o': returnValue = O; break;
  case 'P': returnValue = P; break;
  case 'p': returnValue = P; break;
  case 'Q': returnValue = Q; break;
  case 'q': returnValue = Q; break;
  case 'R': returnValue = R; break;
  case 'r': returnValue = R; break;
  case 'S': returnValue = S; break;
  case 's': returnValue = S; break;
  case 'T': returnValue = T; break;
  case 't': returnValue = T; break;
  case 'U': returnValue = U; break;
  case 'u': returnValue = U; break;
  case 'V': returnValue = V; break;
  case 'v': returnValue = V; break;
  case 'W': returnValue = W; break;
  case 'w': returnValue = W; break;
  case 'X': returnValue = X; break;
  case 'x': returnValue = X; break;
  case 'Y': returnValue = Y; break;
  case 'y': returnValue = Y; break;
  case 'Z': returnValue = Z; break;
  case 'z': returnValue = Z; break;
  }
  return returnValue;
}
 
