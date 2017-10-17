/**
 *  char[] osc_server_new(port_num)
 *
 */

#include "mex.h"
#include "lo/lo.h"

char port[32];
char maddr[32];

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

  int err;
  lo_server s;

  if (nrhs != 1) {
    mexErrMsgTxt("Please specify one arguement, port.");
    return;
  }
  if (nlhs > 1) {
    mexErrMsgTxt("Too many output arguments.");
    return;
  }

  if(! mxIsNumeric(prhs[0])) {
    mexErrMsgTxt("Expecting a numeric scalar in the first argument.");
    return;
  }

  sprintf(port, "%d", (int)(mxGetScalar(prhs[0])));
  s = lo_server_new(port, NULL);  // No error handler...

  if(s == NULL) {
    mexErrMsgTxt("Error creating server.");
    return;
  }

  if(sizeof(void*) == sizeof(long int)) {
    sprintf(maddr, "osc_server:%lx", (void*) s);
  } else if(sizeof(void*) == sizeof(long long int)) {
    sprintf(maddr, "osc_server:%llx", (void*) s);
  } else {
    mexErrMsgTxt("Unsupported pointer size.");
    return;
  }
  
  plhs[0] = mxCreateString(maddr);

}


