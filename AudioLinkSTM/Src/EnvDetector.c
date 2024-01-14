#include "FirFilter.h"
#include "EnvDetector.h"

void EnvDetector_Init(EnvDetector *env)
{
	FirFilter_Init(&(env -> envLPFilter));

	//Inicializacion de los parametros de env
	env -> out = 0.0f;
}
/*
    *fir => puntero a la estructura "fir"
    inp => ultima entrada al filtro
*/
float EnvDetector_Update(EnvDetector *env, float inp)
{
	// Se toma el valor absoluto de la entrada
	if(inp<0){
		inp = -inp;
	}

	env -> out = FirFilter_Update(&(env -> envLPFilter), inp);

    return env -> out;
}



