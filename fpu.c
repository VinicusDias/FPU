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

unionfloat somaValores(unionfloat float1, unionfloat float2) {
    
    unionfloat resultado = float1;
    unsigned int resultadoMantissa;
    unsigned int mantissa1 = float1.field.mantissa + BIT_FANTASMA;
    unsigned int mantissa2 = float2.field.mantissa + BIT_FANTASMA;
    char vaiUm, i, diferencaExpoente = float1.field.expoente - float2.field.expoente;
    
    // Substitui valor2 pelo seu complemento de dois caso os sinais sejam diferentes
    if(float1.field.sinal != float2.field.sinal)
         mantissa2 = ~(mantissa2) + 1;
    
    // Coloca os dois valores sobre o mesmo expoente
    if(diferencaExpoente > 0)
        mantissa2 = mantissa2 >> diferencaExpoente;
    
    // Soma mantissa
    resultadoMantissa = mantissa1 + mantissa2;
    
    // Verifica overflow
    vaiUm = (resultadoMantissa >> 24) & 1;
    
    // Ajusta em caso do resultado negativo
    if(float1.field.sinal != float2.field.sinal && diferencaExpoente == 0 && resultadoMantissa & BIT_FANTASMA && vaiUm == 1){
        resultadoMantissa = ~(resultadoMantissa) +1;
        vaiUm = (resultadoMantissa >> 24) & 1;
    }
    
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
    
    return resultado;
}

main() {
    
    unionfloat float1, float2, resultado;
    
    printf("Primeiro valor: ");
    scanf("%f", &float1.f);
    printf("Segundo valor: ");
    scanf("%f", &float2.f);
    
    // Verifica qual possui maior expoente e inverte caso necessario
    if(float1.field.expoente >= float2.field.expoente)
        resultado = somaValores(float1, float2);
    else
        resultado = somaValores(float2, float1);
    
    printf("Numero reconstituido: %f\n", pow(-1,(resultado.field.sinal)) * ( 1.0 + resultado.field.mantissa / pow(2,23)) * pow(2,(resultado.field.expoente - 127)));
    
}
