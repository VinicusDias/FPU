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

int getBit(unsigned int number, const short int position) {
    if (number & (1 << (position - 1)))
        return 1;
    else
        return 0;
}

int computaSinal(int houveTroca, int houveComplemento, unsigned int sinal1, unsigned int sinal2) {
    //se sinais forem iguais
    if(sinal1 == sinal2) return sinal1;
    
    const struct {
        unsigned int houveTroca : 1;
        unsigned int houveComplemento : 1;
        unsigned int sinal1 : 1;
        unsigned int sinal2 : 1;
    } f = {houveTroca, houveComplemento, sinal1, sinal2};

    if (
            (f.houveTroca && f.sinal1 && !f.sinal2) ||
            (!f.houveTroca && !f.houveComplemento && !f.sinal1 && f.sinal2) ||
            (!f.houveTroca && f.houveComplemento && f.sinal1 && !f.sinal2)
       )
        return 0;
    
    else 
        if (    
                (f.houveTroca && !f.sinal1 && f.sinal2) ||
                (!f.houveTroca && !f.houveComplemento && f.sinal1 && !f.sinal2) ||
                (!f.houveTroca && f.houveComplemento && !f.sinal1 && f.sinal2)
            )
        return 1;

    

}

unionfloat normaliza(unionfloat structNumero, unsigned int mastissa, int short vaiUm) {
    unionfloat resultado = structNumero;
    unsigned int novaMatissa = mastissa;
    //Se tiver vaiUm faz shift para direita e soma expoente
    if (vaiUm) {
        resultado.field.expoente++;
        novaMatissa = novaMatissa >> 1;
    } else {
        //Se não shifta para esquerda até 1.0121 ....
        while (!(novaMatissa & BIT_FANTASMA) && resultado.field.expoente) {
            novaMatissa = novaMatissa << 1;
            resultado.field.expoente--;
        }
    }
    resultado.field.mantissa = novaMatissa;
    return resultado;
}

unionfloat somaValores(unionfloat float1, unionfloat float2, const int troca) {

    const int SINAIS_DIFERENTES = float1.field.sinal != float2.field.sinal;
    int complemento = 0;
    unionfloat resultado = float1;
    unsigned int resultadoMantissa,
            mantissa1 = float1.field.mantissa + BIT_FANTASMA,
            mantissa2 = float2.field.mantissa + BIT_FANTASMA;
    int short vaiUm,
            diferencaExpoente = float1.field.expoente - float2.field.expoente;


    // Substitui valor2 pelo seu complemento de dois caso os sinais sejam diferentes
    if (SINAIS_DIFERENTES) {
        mantissa2 = ~(mantissa2) + 1;       
    }

    // Coloca os dois valores sobre o mesmo expoente
    if (diferencaExpoente > 0)
        mantissa2 = mantissa2 >> diferencaExpoente;

    // Soma mantissa
    resultadoMantissa = mantissa1 + mantissa2;

    // Verifica overflow
    vaiUm = getBit(resultadoMantissa, 25);

    // Ajusta em caso do resultado negativo
    if (SINAIS_DIFERENTES && diferencaExpoente == 0 && (resultadoMantissa & BIT_FANTASMA) && vaiUm) {
        resultadoMantissa = ~(resultadoMantissa) + 1;
        vaiUm = getBit(resultadoMantissa, 25);
        complemento = 1;
    }

    //Normaliza
    resultado = normaliza(resultado, resultadoMantissa, vaiUm);
    resultado.field.sinal = computaSinal(troca, complemento, float1.field.sinal, float2.field.sinal);

    return resultado;

}

unionfloat substracaoValores(unionfloat float1, unionfloat float2, const int troca) {

    const int SINAIS_IGUAIS= float1.field.sinal == float2.field.sinal;
    int complemento = 0;
    unionfloat resultado = float1;
    unsigned int resultadoMantissa,
            mantissa1 = float1.field.mantissa + BIT_FANTASMA,
            mantissa2 = float2.field.mantissa + BIT_FANTASMA;
    int short vaiUm,
            diferencaExpoente = float1.field.expoente - float2.field.expoente;


    // Substitui valor2 pelo seu complemento de dois caso os sinais sejam iguais
    if (SINAIS_IGUAIS) {
        mantissa2 = ~(mantissa2) + 1;       
    }

    // Coloca os dois valores sobre o mesmo expoente
    if (diferencaExpoente > 0)
        mantissa2 = mantissa2 >> diferencaExpoente;

    // Soma mantissa
    resultadoMantissa = mantissa1 + mantissa2;

    // Verifica overflow
    vaiUm = getBit(resultadoMantissa, 25);

    // Ajusta em caso do resultado negativo
    if (SINAIS_IGUAIS && diferencaExpoente == 0 && (resultadoMantissa & BIT_FANTASMA) && vaiUm) {
        resultadoMantissa = ~(resultadoMantissa) + 1;
        vaiUm = getBit(resultadoMantissa, 25);
        complemento = 1;
    }

    //Normaliza
    resultado = normaliza(resultado, resultadoMantissa, vaiUm);
    resultado.field.sinal = computaSinal(troca, complemento, float1.field.sinal, float2.field.sinal);

    return resultado;
}

int main() {

    unionfloat float1, float2, resultado;
    
    float1.f = 0.3;
    float2.f = -3;

    // Verifica qual possui maior expoente e inverte caso necessario
    if (float1.field.expoente >= float2.field.expoente)
        resultado = substracaoValores(float1, float2, 0);
    else
        resultado = substracaoValores(float2, float1, 1);
    
    printf("Numero reconstituido: %f\n", pow(-1, (resultado.field.sinal)) * (1.0 + resultado.field.mantissa / pow(2, 23)) * pow(2, (resultado.field.expoente - 127)));
    return 0;
}
