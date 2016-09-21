#include "stdafx.h"
#include "CoupledContinuousMaxFlow.h"
#include "CoupledContinuousMaxFlowParams.h"
#include <stdlib.h>
#include <exception>


#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <time.h>


#define YES 0
#define NO 1

#define PI 3.1415926

#define MAX(a,b) ( a > b ? a : b )
#define MIN(a,b) ( a <= b ? a : b )
#define SIGN(x) ( x >= 0.0 ? 1.0 : -1.0 )
#define ABS(x) ( (x) > 0.0 ? x : -(x) )
#define SQR(x) (x)*(x)

#ifndef HAVE_RINT 
#define rint(A) floor((A)+(((A) < 0)? -0.5 : 0.5)) 
#endif

float SQRT(float number) {
	long i;
	float x, y;
	const float f = 1.5F;

	x = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;
	i  = 0x5f3759df - ( i >> 1 );
	y  = * ( float * ) &i;
	y  = y * ( f - ( x * y * y ) );
	y  = y * ( f - ( x * y * y ) );
	return number * y;
}
 

bool CoupledContinuousMaxFlow::DoMaxFlow( float* Cs,float* Ct,float* penalty,CoupledContinuousMaxFlowParams params,float*& outImage )
{

  /* iNbOut: number of outputs
     pmxOut: array of pointers to output arguments */
    
  /* iNbIn: number of inputs
     pmxIn: array of pointers to input arguments */
    
    
    float   *pfu;
    float   *pfqq, *pfVecParameters;
    float   *pfbx, *pfby, *pfps;
    float   *pfpt, *pfgk, *tt, *pfdv;
    float   fLambda, fError, cc, steps, beta;
    float   fpt, fps;
    int     *punum, iNy, iNx, iNdim, iDim[3], ix, iy, i, iNI, nSlices;
    int     iNbIters, szImg, ntImg, idx, index, idz, idx_r, index_r, index_q;
        
    /* 
     *pfVecParameters Setting
     * [0] : number of columns 
     * [1] : number of rows
     * [2] : the maximum iteration number
     * [3] : error criterion
     * [4] : cc for the step-size of ALM
     * [5] : steps for the step-size of projected-gradient of p
     * [6] : penalty for the results between neighbour slices
     * [7] : the number of slices
     */
    
    /* Size */
    iNy = params.Ny;
    iNx = params.Nx;
    szImg = iNy * iNx;
    
    iNbIters = params.NbIters;
    fError = params.fError;
    cc = params.cc;
    steps =params.steps;
    beta = params.beta;
    nSlices = params.nSlices;
    ntImg = (nSlices-1)*szImg;
    
    /* Outputs */
    /* outputs the computed u(x)  */
    iNdim = 3;
    iDim[0] = iNy;
    iDim[1] = iNx;
    iDim[2] = nSlices;
    
     /* Memory allocation */
	pfu = (float *) calloc( (unsigned)(iNy*iNx*nSlices), sizeof(float) );
	if (!pfu)
		throw (std::bad_alloc());

    /* allocate the memory for px1 and px2 */
    pfbx = (float *) calloc( (unsigned)(iNy*(iNx+1)*nSlices), sizeof(float) );
    if (!pfbx)
       throw (std::bad_alloc());
    /* allocate the memory for py1 and py2 */
    pfby = (float *) calloc( (unsigned)((iNy+1)*iNx*nSlices), sizeof(float) );
    if (!pfby)
		throw (std::bad_alloc());
    /* allocate the memory for ps1 and ps2 */
    pfps = (float *) calloc( (unsigned)(iNy*iNx)*nSlices, sizeof(float) );
    if (!pfps)
      throw (std::bad_alloc());
    /* allocate the memory for pt1 and pt2 */
    pfpt = (float *) calloc( (unsigned)(iNy*iNx*nSlices), sizeof(float) );
    if (!pfpt)
        throw (std::bad_alloc());
    /* allocate the memory for the coupled flow q */
    pfqq = (float *) calloc( (unsigned)(iNy*iNx*nSlices), sizeof(float) );
    if (!pfqq)
        throw (std::bad_alloc());
    
    /* allocate the memory for gk */
    pfgk = (float *) calloc( (unsigned)(iNy*iNx*nSlices), sizeof(float) );
    if (!pfgk)
      throw (std::bad_alloc());
    
    /* allocate the memory for div1 and div2 */
    pfdv = (float *) calloc( (unsigned)(iNy*iNx*nSlices), sizeof(float) );
    if (!pfdv)
         throw (std::bad_alloc());
    
    /* Preparing initial values for flows and u */
    for (i = 0; i < nSlices; i ++){
        idz = i*szImg;
        for (ix=0; ix< iNx; ix++){
            idx = idz + ix*iNy;
            for (iy=0; iy< iNy; iy++){
               index = idx + iy; 

                if (Cs[index] < Ct[index]){
                    pfps[index] = Cs[index];
                    pfpt[index] = Cs[index];
                    pfdv[index] = pfbx[index+iNy] - pfbx[index] 
                             + pfby[index+1] - pfby[index];
                }
                else{
                    pfu[index] = 1;
                    pfps[index] = Ct[index];
                    pfpt[index] = Ct[index];
                    pfdv[index] = pfbx[index+iNy] - pfbx[index] 
                             + pfby[index+1] - pfby[index];
                }
            }
        }
    }
    
    /*  Main iterations */
    
    iNI = 0;  
    while( iNI<iNbIters ) 
    { 
        
        /* update the spatial flow field px and py */
        
        for (ix=0; ix< iNx; ix++){
            idx = ix*iNy;
            idx_r = ntImg + (iNx-1-ix)*iNy;
            
            for (iy=0; iy< iNy; iy++){
                index = idx + iy;
                index_r = idx_r + iy;
                pfgk[index] = pfdv[index] - (pfps[index] - pfpt[index] 
                             + pfqq[index] - pfqq[index_r]+ pfu[index]/cc);

            }
        }
        
        for (i = 1; i < nSlices; i ++){
            idz = i * szImg;
            for (ix=0; ix< iNx; ix++){
                idx = ix*iNy + idz;
                for (iy=0; iy< iNy; iy++){
                    index = idx + iy;
                    pfgk[index] = pfdv[index] - (pfps[index] - pfpt[index] 
                                 + pfqq[index] - pfqq[index - szImg]+ pfu[index]/cc);

                }
            }
        }
        
        /* update px */
        for (i = 0; i < nSlices; i ++){
            idz = i * szImg;
            for (ix=0; ix < iNx-1; ix++){
                idx = idz + (ix+1)*iNy;
                for (iy=0; iy < iNy; iy++){
                    index = idx + iy;
                    pfbx[index] = steps*(pfgk[index] - pfgk[index-iNy]) + pfbx[index];
                }
            }
        }
    
        /* update py */
         for (i = 0; i < nSlices; i ++){
            idz = i * szImg;
            for(ix = 0; ix < iNx; ix ++){
                idx = idz + ix*iNy;
                for(iy = 0; iy < iNy-1; iy ++){
                    index = idx + iy + 1;
                    pfby[index] = steps*(pfgk[index] - pfgk[index-1]) + pfby[index];
                }
            }
         }
        
        /* projection step */
         for (i = 0; i < nSlices; i ++){
            idz = i * szImg;
            for (ix = 0; ix < iNx; ix++){
                idx = idz + ix*iNy;
                for (iy = 0; iy < iNy; iy++){
                    index = idx + iy;
                    fpt = SQRT((SQR(pfbx[index]) + SQR(pfbx[index+iNy]) 
                          + SQR(pfby[index]) + SQR(pfby[index+1]))*0.5);

                    if (fpt > penalty[index])
                        pfgk[index] = penalty[index]/fpt;
                    else
                        pfgk[index] = 1;
                }
            }
         }
        
        for (i = 0; i < nSlices; i ++){
            idz = i * szImg;
            for (ix =0; ix < iNx-1; ix++){
                idx = idz + (ix+1)*iNy;
                for (iy = 0; iy < iNy; iy++){
                    index = idx + iy;
                    pfbx[index] = (pfgk[index] + pfgk[index-iNy])*0.5*pfbx[index];
                }
            }
        }
        
        for (i = 0; i < nSlices; i ++){
            idz = i * szImg;
            for (ix = 0; ix < iNx; ix++){
                idx = idz + ix*iNy;
                for (iy = 0; iy < iNy-1; iy++){
                    index = idx + iy + 1;
                    pfby[index] = (pfgk[index-1] + pfgk[index])*0.5*pfby[index];
                }
            }      
        }
        
        /* compute the divergence  */
        for (i = 0; i < nSlices; i ++){
            idz = i * szImg;
            for (ix = 0; ix < iNx; ix++){
                idx = idz + ix*iNy;
                for (iy = 0; iy < iNy; iy++){
                    index = idx + iy;

                    pfdv[index] = pfbx[index+iNy] - pfbx[index] 
                             + pfby[index+1] - pfby[index];
                }
            }
        }                
        
        /* update ps */
        for (ix=0; ix< iNx; ix++){
            idx = ix*iNy;
            idx_r = ntImg + (iNx-1-ix)*iNy;
            
            for (iy=0; iy< iNy; iy++){
                index = idx + iy;
                index_r = idx_r + iy;
                fpt = pfdv[index] + pfpt[index] 
                     - pfqq[index] + pfqq[index_r] - pfu[index]/cc + 1/cc;
                pfps[index] = MIN(fpt , Cs[index]);
            }
        }
        
        for (i = 1; i < nSlices; i ++){
            idz = i * szImg;
            for (ix = 0; ix < iNx; ix++){
                idx = idz + ix*iNy;
                for (iy = 0; iy < iNy; iy++){
                    index = idx + iy;
                    fpt = pfpt[index] - pfu[index]/cc + pfdv[index] - pfqq[index] + pfqq[index - szImg] + 1/cc;
                    pfps[index] = MIN(fpt , Cs[index]);
                }
            }
        }
        
        /* update pt */
        
        for (ix=0; ix< iNx; ix++){
            idx = ix*iNy;
            idx_r = ntImg + (iNx-1-ix)*iNy;
            
            for (iy=0; iy< iNy; iy++){
                index = idx + iy;
                index_r = idx_r + iy;
                fpt = - pfdv[index] + pfps[index] 
                     + pfqq[index] - pfqq[index_r] + pfu[index]/cc;
                pfpt[index] = MIN(fpt , Ct[index]);
            }
        }
        
        for (i = 1; i < nSlices; i ++){
            idz = i * szImg;
            for (ix = 0; ix < iNx; ix++){
                idx = idz + ix*iNy;
                for (iy = 0; iy < iNy; iy++){
                    index = idx + iy;
                    fpt = pfps[index] + pfu[index]/cc - pfdv[index] + pfqq[index] - pfqq[index-szImg];
                    pfpt[index] = MIN(fpt , Ct[index]);
                }
            }
        }
        
        /* update the coupled flow qq */
         for (i = 0; i < nSlices; i ++){
            idz = i * szImg;
            for (ix = 0; ix < iNx; ix++){
                idx = idz + ix*iNy;
                for (iy = 0; iy < iNy; iy++){
                    index = idx + iy;
                    pfgk[index] = - pfps[index] + pfdv[index] + pfpt[index];
                }
            }
        }
        
        for (ix=0; ix< iNx; ix++){
            idx = ix*iNy;
            idx_r = ntImg + (iNx-1-ix)*iNy;
            
            for (iy=0; iy< iNy; iy++){
                index = idx + iy;
                index_r = idx_r + iy;
                index_q = index + szImg;
                
                fpt = ((pfgk[index] - pfu[index]/cc + pfqq[index_r]) -
                   (pfgk[index_q] - pfu[index_q]/cc - pfqq[index_q]))/2;
                pfqq[index] = MAX(MIN(fpt , beta),-beta);
            }
        }        
        
        for (i = 1; i < nSlices-1; i ++){
            idz = i * szImg;
            for (ix = 0; ix < iNx; ix++){
                idx = idz + ix*iNy;
                for (iy = 0; iy < iNy; iy++){
                    index = idx + iy;
                    index_r = index + szImg;
                    fpt = ((pfgk[index] - pfu[index]/cc + pfqq[index-szImg]) -
                         (pfgk[index_r] - pfu[index_r]/cc - pfqq[index_r]))/2;
                    pfqq[index] = MAX(MIN(fpt , beta), -beta);
                }
            }
        }
        
        for (ix=0; ix< iNx; ix++){
            idx_r = (iNx-1-ix)*iNy;
            idx = ntImg + ix*iNy;
            
            for (iy=0; iy< iNy; iy++){
                index = idx + iy;
                index_r = idx_r + iy;
                
                fpt = (-(pfgk[index_r] - pfu[index_r]/cc - pfqq[index_r]) +
                   (pfgk[index] - pfu[index]/cc + pfqq[index-szImg]))/2;
                pfqq[index] = MAX(MIN(fpt , beta),-beta);
            }
        } 
        
        /* update multipliers */
        
        fps = 0;
        
        for (ix=0; ix< iNx; ix++){
            idx = ix*iNy;
            idx_r = ntImg + (iNx-1-ix)*iNy;
            
            for (iy=0; iy< iNy; iy++){
                index = idx + iy;
                index_r = idx_r + iy;
                
                fpt = cc*(pfgk[index] - pfqq[index] + pfqq[index_r]);
                fps += ABS(fpt);
                pfu[index] -= fpt;
            }
        }
        
         for (i = 1; i < nSlices; i ++){
            idz = i * szImg;
            for (ix = 0; ix < iNx; ix++){
                idx = idz + ix*iNy;
                for (iy = 0; iy < iNy; iy++){
                    index = idx + iy;

                    fpt = cc*(pfgk[index] - pfqq[index] + pfqq[index-szImg]);
                    fps += ABS(fpt);
                    pfu[index] -= fpt;
                }
            }
         }        
		 double currError = fps / szImg / nSlices;

		 if (currError <= fError)
			 break;
       
                
        iNI ++;
     }   
    
    
    outImage = pfu;
	
    
	/* Free memory */
    
    free( (float *) pfbx );
    free( (float *) pfby );
    free( (float *) pfps );
    free( (float *) pfpt );
    free( (float *) pfgk );
    free( (float *) pfdv );
    free( (float *) pfqq );
    
    
}