/*
 * GPisMap - Online Continuous Mapping using Gaussian Process Implicit Surfaces
 * https://github.com/leebhoram/GPisMap
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v3 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of any FITNESS FOR A
 * PARTICULAR PURPOSE. See the GNU General Public License v3 for more details.
 *
 * You should have received a copy of the GNU General Public License v3
 * along with this program; if not, you can access it online at
 * http://www.gnu.org/licenses/gpl-3.0.html.
 *
 * Authors: Bhoram Lee <bhoram.lee@gmail.com>
 *          Huang Zonghao<ac@hzh.io>
 */

#include "mex.h"
#include <vector>
#include <memory>
#include <iostream>
#include <chrono>
#include "GPisMap2.h"

static GPisMap2* gpm = 0;

void mexFunction (int nlhs,mxArray *plhs[],int nrhs,const mxArray *prhs[]) {

    char command[128];
    mxGetString(prhs[0],command,128);
    std::string commstr(command);

    if (commstr.compare("update")==0) {

        if (gpm == 0){
            gpm = new GPisMap2();
        }
        mwSize numDim = mxGetNumberOfDimensions(prhs[1]);
        const mwSize *dims = mxGetDimensions(prhs[1]);

        if ( numDim == 2 && (nrhs == 3))
        {
            if (mxSINGLE_CLASS != mxGetClassID(prhs[1]) ||
                mxSINGLE_CLASS != mxGetClassID(prhs[2]) ){
                std::cout << "The input data must be float (single) type.  @ train()" << std::endl;
                return;
            }

            float * pz = (float *)mxGetData(prhs[1]); // 3d points
            float * ppose = (float *)mxGetData(prhs[2]); //poses

            std::vector<float> pose(ppose, ppose + mxGetNumberOfElements(prhs[2]) );

            int numel =  mxGetNumberOfElements(prhs[1]);
            gpm->update(pz, numel, pose);


        }
        else
            std::cout << "Error: Check the input dimension." << std::endl;
        return;
    }
    else if (commstr.compare("test")==0) {
        const mwSize *dims = mxGetDimensions(prhs[1]);
        int dim = dims[0];
        int N = dims[1];

        if (dim !=3)
            return;

            if (mxSINGLE_CLASS != mxGetClassID(prhs[1])){
                std::cout << "The input data must be float (single) type. @ test()" << std::endl;
                return;
            }

        if ((gpm != 0) && (N > 0)){
            float* px = (float *)mxGetData(prhs[1]);

            if (sizeof(float) == sizeof(double)){
                plhs[0] = mxCreateDoubleMatrix(2*(1+dim),N,mxREAL);   // f-value[0],  grad-value[1,2], variances[3-5]
            }
            else if (sizeof(float) == sizeof(float)){
                plhs[0] = mxCreateNumericMatrix(2*(1+dim),N,mxSINGLE_CLASS, mxREAL);
            }

            float *pRes   = (float*)mxGetPr(plhs[0]);
            gpm->test(px, dim, N, pRes);

        }
        else if (gpm ==0){
            std::cout << "Error: the map is not initialized." << std::endl;
        }

    }   
 
    else if (commstr.compare("getAllPoints")==0){
        if (gpm != 0){
            std::vector<float> pos;
            gpm->getAllPoints(pos);

            int N = pos.size()/3; // 2D
            if (N > 0){
                plhs[0] = mxCreateNumericMatrix(3,N,mxSINGLE_CLASS, mxREAL);
                float *pRes   = (float*)mxGetPr(plhs[0]);
                memcpy(pRes,pos.data(),sizeof(float)*3*N);
            }
        }
    }
    else if (commstr.compare("reset")==0) {
        if (gpm != 0){
            gpm->reset();
            delete gpm;
            gpm = 0;
            std::cout << "Map cleared\n" << std::endl;
        }
        return;
    }

    return;
}
