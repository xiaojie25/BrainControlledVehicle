#include "fdacoefs.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>

const int NL[MWSPT_NSEC] = { 1,3,1,3,1,3,1,3,1 };
const real64_T NUM_1K[MWSPT_NSEC][3] = {
  {
     0.9945258815962,                 0,                 0 
  },
  {
                   1,   -1.902450998481,                 1 
  },
  {
     0.9945258815962,                 0,                 0 
  },
  {
                   1,   -1.902450998481,                 1 
  },
  {
     0.9870718881864,                 0,                 0 
  },
  {
                   1,   -1.902450998481,                 1 
  },
  {
     0.9870718881864,                 0,                 0 
  },
  {
                   1,   -1.902450998481,                 1 
  },
  {
                   1,                 0,                 0 
  }
};
const real64_T NUM_250[MWSPT_NSEC][3] = {
  {
     0.9777833971339,                 0,                 0 
  },
  {
                   1,  -0.6197948878442,                 1 
  },
  {
     0.9777833971339,                 0,                 0 
  },
  {
                   1,  -0.6197948878442,                 1 
  },
  {
     0.9506464692387,                 0,                 0 
  },
  {
                   1,  -0.6197948878442,                 1 
  },
  {
     0.9506464692387,                 0,                 0 
  },
  {
                   1,  -0.6197948878442,                 1 
  },
  {
                   1,                 0,                 0 
  }
};

const int DL[MWSPT_NSEC] = { 1,3,1,3,1,3,1,3,1 };
const real64_T DEN_1K[MWSPT_NSEC][3] = {
  {
                   1,                 0,                 0 
  },
  {
                   1,   -1.883429954662,    0.988789958782 
  },
  {
                   1,                 0,                 0 
  },
  {
                   1,   -1.900323233102,    0.989654204663 
  },
  {
                   1,                 0,                 0 
  },
  {
                   1,   -1.874119761215,   0.9737465573738 
  },
  {
                   1,                 0,                 0 
  },
  {
                   1,   -1.881538306593,   0.9745991660985 
  },
  {
                   1,                 0,                 0 
  }
};
const real64_T DEN_250[MWSPT_NSEC][3] = {
  {
                   1,                 0,                 0 
  },
  {
                   1,  -0.5088780931146,    0.957490655514 
  },
  {
                   1,                 0,                 0 
  },
  {
                   1,  -0.7016226883609,   0.9588688936529 
  },
  {
                   1,                 0,                 0 
  },
  {
                   1,  -0.5501134078213,   0.9010772205295 
  },
  {
                   1,                 0,                 0 
  },
  {
                   1,  -0.6280540375804,   0.9023832132441 
  },
  {
                   1,                 0,                 0 
  }
};


void IIRFilter_Init(IIRFilter *filt, const real64_T NUM[MWSPT_NSEC][3], const real64_T DEN[MWSPT_NSEC][3]){
    for (int i = 0; i < MWSPT_NSEC; i++) {
        filt->sections[i].b0 = NUM[i][0];
        filt->sections[i].b1 = NUM[i][1];
        filt->sections[i].b2 = NUM[i][2];
        filt->sections[i].a1 = -DEN[i][1];
        filt->sections[i].a2 = -DEN[i][2];
        filt->sections[i].x1 = filt->sections[i].x2 = 0;
        filt->sections[i].y1 = filt->sections[i].y2 = 0;
    }
}

real64_T  IIRFilter_Process(IIRFilter *filt, real64_T input){
    real64_T output = input;

    for (int i = 0; i < MWSPT_NSEC; i++) {
        Biquad *bq = &filt->sections[i];

        real64_T yn = bq->b0 * output + 
                      bq->b1 * bq->x1 + 
                      bq->b2 * bq->x2 + 
                      bq->a1 * bq->y1 + 
                      bq->a2 * bq->y2;
				bq->x2 = bq->x1;     
				bq->x1 = output;     
				bq->y2 = bq->y1;     
				bq->y1 = yn;          
        output = yn;
    }

    return output;
}

int Decoder(const void * Input, size_t Len){
	const uint8_t * Temp = (const uint8_t *)Input;
	if(Len != 3){
		return -1;
	}
	uint32_t Combined = (uint32_t)((Temp[0] << 16)|(Temp[1] << 8)|(Temp[2]));
	if(Combined & 0x800000){
		return ((int)(Combined | 0xFF000000));
	}else{
		return (int)Combined;
	}
}

void Encoding(const void * Input, uint8_t * Output){
	const int32_t * Temp_ptr = (const int32_t *)Input;
	int32_t Temp = *Temp_ptr;
	
	Temp = Temp & 0xFFFFFF;
	Output[0] = (Temp >> 16) & 0xFF;  
  Output[1] = (Temp >> 8) & 0xFF;   
  Output[2] = Temp & 0xFF;         
	
	return;
}
