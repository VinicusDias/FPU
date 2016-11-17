#include <stdio.h>
#include <math.h>

#define BIT_FANTASMA 8388608

typedef union {
	float f;
	struct {
		unsigned int mantissa : 23;
		unsigned int expoente : 8;
		unsigned int sinal : 1;
	} field;
} unionfloat;

unsigned int computaSinal(int houveTroca, int houveComplemento, unsigned int sinal1, unsigned int sinal2) {
    
    if(sinal1 == sinal2 || houveTroca) return sinal1;
    
    if(houveComplemento) return  sinal2;
    
    else
        return sinal1;

}

unionfloat processaValores(unionfloat float1, unionfloat float2, char operacao) {
    
    unionfloat resultado = float1;
    unsigned int resultadoMantissa,
                mantissa1,
                mantissa2;
    char vaiUm,
        diferencaExpoente,
        complemento,
        troca;
    
    // Verifica valores nulos
    if(float1.field.expoente == 0 && float1.field.mantissa == 0 )
        return float2;
        
    if(float2.field.expoente == 0 && float2.field.mantissa == 0 )
        return float1;
    
    // Verifica qual possui maior expoente e inverte caso necessario
    if(float1.field.expoente < float2.field.expoente){
        resultado = float2;
        float2 = float1;
        float1 = resultado;
        troca = 1;
    }
    else
        troca = 0;
    
    mantissa1 = float1.field.mantissa + BIT_FANTASMA;
    mantissa2 = float2.field.mantissa + BIT_FANTASMA;
    diferencaExpoente = float1.field.expoente - float2.field.expoente;
    
    
    // Substitui valor2 pelo seu complemento de dois caso os sinais sejam diferentes
    if((float1.field.sinal != float2.field.sinal && operacao == 0) || (float1.field.sinal == float2.field.sinal && operacao == 1)){
        mantissa2 = ~(mantissa2) + 1;
    }
    
     // Coloca os dois valores sobre o mesmo expoente
    if(diferencaExpoente > 0)
        mantissa2 = mantissa2 >> diferencaExpoente;
    
    // Soma mantissa
    resultadoMantissa = mantissa1 + mantissa2;
    
     // Verifica overflow
    vaiUm = (resultadoMantissa >> 24) & 1;
    
    // Complemento de 2 em caso do resultado negativo
    if(float1.field.sinal != float2.field.sinal && diferencaExpoente == 0 && resultadoMantissa & BIT_FANTASMA && vaiUm){
        resultadoMantissa = ~(resultadoMantissa) +1;
        vaiUm = (resultadoMantissa >> 24) & 1;
        complemento = 1;
    }
    else
        complemento = 0;
    
    // Normaliza
    if(vaiUm == 1){
        resultado.field.expoente++;
        resultadoMantissa = resultadoMantissa >> 1;
    }
    else while(!(resultadoMantissa & BIT_FANTASMA) && resultado.field.expoente){
        resultadoMantissa = resultadoMantissa << 1;
        resultado.field.expoente--;
    }
    
    resultado.field.mantissa = resultadoMantissa;
    
    // Ajusta o sinal
    resultado.field.sinal = computaSinal(troca, complemento, float1.field.sinal, float2.field.sinal);
    
    if(operacao == 1 && troca == 1)
        resultado.field.sinal = ~(resultado.field.sinal);
    
    return resultado;
}

void main() {
    
    unionfloat float1, float2, resultado;
    
    float1.f = 9.75;
    float2.f = -0.5625;
    
    resultado = processaValores(float1, float2, 0);
    
    printf("Numero reconstituido: %f\n", pow(-1,(resultado.field.sinal)) * ( 1.0 + resultado.field.mantissa / pow(2,23)) * pow(2,(resultado.field.expoente - 127)));
    
}
