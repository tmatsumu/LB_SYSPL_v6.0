//#include "/usr/include/python2.7/Python.h"
// //#include "/usr/share/pyshared/numpy/core/include/numpy/arrayobject.h"
// //#include "/usr/share/pyshared/numpy/core/include/numpy/arrayscalars.h"
//#include "/usr/lib64/python2.7/site-packages/numpy/core/include/numpy/arrayobject.h"
//#include "/usr/lib64/python2.7/site-packages/numpy/core/include/numpy/arrayscalars.h"

//#include "/group/cmb/litebird/common_tools/opt/include/python2.7/Python.h"
//#include "/group/cmb/litebird/common_tools/opt/lib/python2.7/site-packages/numpy-1.12.0.dev0+a93d9f7-py2.7-linux-x86_64.egg/numpy/core/include/numpy/arrayobject.h"
//#include "/group/cmb/litebird/common_tools/opt/lib/python2.7/site-packages/numpy-1.12.0.dev0+a93d9f7-py2.7-linux-x86_64.egg/numpy/core/include/numpy/arrayscalars.h"

#include "/usr/include/python2.7/Python.h"
#include "/usr/lib64/python2.7/site-packages/numpy/core/include/numpy/arrayobject.h"
#include "/usr/lib64/python2.7/site-packages/numpy/core/include/numpy/arrayscalars.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//##############################################################################################
double **ptrvector(int n)  {
  double **v;
  v=(double **)malloc((size_t) (n*sizeof(double)));
  if (!v)   {
    printf("In **ptrvector. Allocation of memory for double array failed.");
    exit(0);  }
  return v;
}

PyArrayObject *pymatrix(PyObject *objin)  {
  return (PyArrayObject *) PyArray_ContiguousFromObject(objin,NPY_DOUBLE, 2,2);
}

double *pyvector_to_Carrayptrs(PyArrayObject *arrayin){
  int n;
  n=arrayin->dimensions[0];
  return (double *) arrayin->data;  /* pointer to arrayin data as double */
}

int *pyvector_to_Carrayptrs_int(PyArrayObject *arrayin){
  int n;
  n=arrayin->dimensions[0];
  return (int *) arrayin->data;  /* pointer to arrayin data as double */
}

double **pymatrix_to_Carrayptrs(PyArrayObject *arrayin)  {
  double **c, *a;
  int i,n,m;
  
  n=arrayin->dimensions[0];
  m=arrayin->dimensions[1];
  c=ptrvector(n);
  a=(double *) arrayin->data;  /* pointer to arrayin data as double */
  for ( i=0; i<n; i++)  {
    c[i]=a+i*m;  }
  return c;
}

void free_Carrayptrs(double *v){
  free((char*) v);
}

void free_Cint2Darrayptrs(double **v)  {
  free((char*) v);
}

int not_doublevector(PyArrayObject *vec){
  if (vec->descr->type_num != NPY_DOUBLE || vec->nd != 1)  {
    PyErr_SetString(PyExc_ValueError,
		    "In not_doublevector: array must be of type Float and 1 dimensional (n).");
    return 1;  }
  return 0;
}

//##############################################################################################
void matrix2x2_multi_xy( double *x,  double *y,  double phi)
{
  double tmp;
  tmp =  cos(phi) * (*x) - sin(phi) * (*y);
  *y = sin(phi) * (*x) + cos(phi) * (*y);
  *x = tmp;
}

void matrix2x2_multi_xz( double *x,  double *z,  double theta)
{
  double tmp;
  tmp =  cos(theta)* (*x) + sin(theta)* (*z);
  *z = -sin(theta)* (*x) + cos(theta)* (*z);
  *x = tmp;
}

double wraparound_npi(double x, int n_)
{
  double tmp;
  int n_wrap;
  n_ = (float)n_;
  tmp = x/(n_*M_PI);
  n_wrap = (int)tmp;
  if (x<0) n_wrap-=1; 
  return x-n_wrap*n_*M_PI;
}

void deriv_theta( double xi, double yi, double zi, 
		  double *xo, double *yo, double *zo)
{
  double theta, phi;
  theta = atan(sqrt(xi*xi+yi*yi)/zi)+5.*M_PI; // -pi/2 ~ pi/2
  theta = wraparound_npi(theta,1);
  phi = atan2(yi,xi)+10.*M_PI; //  -pi ~ pi 
  phi = wraparound_npi(phi,2);
  *xo =   cos(theta) * cos(phi);
  *yo =   cos(theta) * sin(phi);
  *zo = - sin(theta);
}

void deriv_phi(double xi, double yi, double zi,
	       double *xo, double *yo, double *zo)
{ 
  double theta, phi;
  theta = atan(sqrt(xi*xi+yi*yi)/zi);
  theta = wraparound_npi(theta,1);
  phi = atan2(yi,xi)+10*M_PI; //# -pi ~ pi 
  phi = wraparound_npi(phi,2);
  *xo = - sin(phi);
  *yo =   cos(phi);
  *zo =   0.;
}

void vec2ang(double xi, double yi, double zi, 
	     double *theta_out, double *phi_out)
{
  *theta_out = wraparound_npi( atan( sqrt(xi*xi+yi*yi)/zi )+5.*M_PI, 1);
  *phi_out = wraparound_npi( atan2(yi,xi)+10.*M_PI, 2);
}

double cosangle(double xi, double yi, double zi,
		double xii, double yii, double zii)
{
  // output is always within 0-pi
  return acos((xi*xii+yi*yii+zi*zii)/sqrt(xi*xi+yi*yi+zi*zi)/sqrt(xii*xii+yii*yii+zii*zii));
}

//##############################################################################################
static PyObject *LB_rotmatrix_multi(PyObject *self, PyObject *args)
{
  /* basic variables */
  long i;
  int num, dims[2];
  double pi=M_PI;
  //  double pi=4.*atan(1.), radeg=(180./pi);
  
  /* variables for inputs */
  PyArrayObject *theta_asp, *phi_asp; // theta and phi of anti-sun vector
  PyArrayObject *phi_antisun, *phi_boresight; // precession and spin axes of satellite rotation
  double theta_antisun, theta_boresight;

  /* variable for outputs */
  PyArrayObject *pout;
  
  /* Parse tuples separately since args will differ between C fcns */
  if (!PyArg_ParseTuple(args, "O!O!dO!dO!", 
			&PyArray_Type, &theta_asp, &PyArray_Type, &phi_asp,
			&theta_antisun, &PyArray_Type, &phi_antisun,
			&theta_boresight, &PyArray_Type, &phi_boresight))  return NULL;
  if (NULL == theta_asp)  return NULL;
  if (NULL == phi_asp)  return NULL;
  if (NULL == phi_antisun)  return NULL;
  if (NULL == phi_boresight)  return NULL;
  
  /* Check that objects are 'double' type and vectors
     Not needed if python wrapper function checks before call to this routine */
  if (not_doublevector(theta_asp)) return NULL;
  if (not_doublevector(phi_asp)) return NULL;
  if (not_doublevector(phi_antisun)) return NULL;
  if (not_doublevector(phi_boresight)) return NULL;
  
  /* Get vector dimension. */
  dims[0] = 3;
  num=dims[1]=theta_asp->dimensions[0];
  pout=(PyArrayObject *) PyArray_FromDims(2,dims,NPY_DOUBLE);

  /* Change contiguous arrays into C * arrays   */
  double *c_theta_asp, *c_phi_asp;
  double *c_phi_antisun, *c_phi_boresight; // precession axis, spin axis
  double **c_pout;
  c_theta_asp = pyvector_to_Carrayptrs(theta_asp);
  c_phi_asp = pyvector_to_Carrayptrs(phi_asp);

  c_phi_antisun = pyvector_to_Carrayptrs(phi_antisun);
  c_phi_boresight = pyvector_to_Carrayptrs(phi_boresight);
  c_pout = pymatrix_to_Carrayptrs(pout);

  //  printf("     > %lf %lf \n  ", theta_antisun/pi*180., theta_boresight/pi*180.);
  double xi, yi, zi, rel_phi;
  for(i=0;i<num;i++){
    
    xi = sin(c_theta_asp[i])*cos(c_phi_asp[i]);
    yi = sin(c_theta_asp[i])*sin(c_phi_asp[i]);
    zi = cos(c_theta_asp[i]);
    
    if (yi >= 0) rel_phi = acos(xi);
    if (yi < 0) rel_phi = -acos(xi) + 2.*pi;
    matrix2x2_multi_xy( &xi, &yi,  rel_phi);
    matrix2x2_multi_xz( &xi, &zi,  pi/2.);
    matrix2x2_multi_xz( &xi, &zi, -theta_boresight);
    matrix2x2_multi_xy( &xi, &yi, -c_phi_boresight[i]);
    matrix2x2_multi_xz( &xi, &zi, -theta_antisun);
    matrix2x2_multi_xy( &xi, &yi, -c_phi_antisun[i]);
    matrix2x2_multi_xz( &xi, &zi, -pi/2.);
    matrix2x2_multi_xy( &xi, &yi, -rel_phi);
    
    c_pout[0][i] = xi;
    c_pout[1][i] = yi;
    c_pout[2][i] = zi;
  }
  
//  free_Carrayptrs(c_theta_asp);
//  free_Carrayptrs(c_phi_asp);
//  free_Carrayptrs(c_phi_antisun);
//  free_Carrayptrs(c_phi_boresight);
  //  free_Carrayptrs(c_pout);
  //  printf("=> %lf %lf %lf \n", c_pout[0][200], c_pout[1][200], c_pout[2][200]);
  //  return Py_BuildValue("i", 1);
  return PyArray_Return(pout);
}
//##############################################################################################
// include the alpha angle (or psi angle) to be an output of this function in addition to 
//  the nominal output P_out in LB_rotmatrix_multi()
//  modified in 2017-11-1 by Tomo based on the discussion with Guillaume
static PyObject *LB_rotmatrix_multi2(PyObject *self, PyObject *args)
{
  /* basic variables */
  long i;
  int num, dims[2];
  double pi=M_PI;
  //  double pi=4.*atan(1.), radeg=(180./pi);
  
  /* variables for inputs */
  PyArrayObject *theta_asp, *phi_asp; // theta and phi of anti-sun vector
  //  PyArrayObject *phi_antisun, *phi_boresight; // precession and spin axes of satellite rotation
  double theta_antisun, theta_boresight;
  double omega_pre, omega_spin;
  PyArrayObject *time;

  /* variable for outputs */
  PyArrayObject *pout;
  
  /* Parse tuples separately since args will differ between C fcns */
  // O: object, !: array, d: double
  if (!PyArg_ParseTuple(args, "O!O!ddddO!", 
			&PyArray_Type, &theta_asp, 
			&PyArray_Type, &phi_asp,
			&theta_antisun, &theta_boresight, 
			&omega_pre, &omega_spin,
			&PyArray_Type, &time))  return NULL;
  if (NULL == theta_asp)  return NULL;
  if (NULL == phi_asp)  return NULL;
  if (NULL == time) return NULL;
  
  /* Check that objects are 'double' type and vectors
     Not needed if python wrapper function checks before call to this routine */
  if (not_doublevector(theta_asp)) return NULL;
  if (not_doublevector(phi_asp)) return NULL;
  if (not_doublevector(time)) return NULL;
  
  /* Get vector dimension. */
  dims[0] = 4;
  num=dims[1]=theta_asp->dimensions[0];
  pout=(PyArrayObject *) PyArray_FromDims(2,dims,NPY_DOUBLE);

  /* Change contiguous arrays into C * arrays   */
  double *c_theta_asp, *c_phi_asp;
  double *c_time;
  double **c_pout;
  c_theta_asp = pyvector_to_Carrayptrs(theta_asp);
  c_phi_asp = pyvector_to_Carrayptrs(phi_asp);
  c_time = pyvector_to_Carrayptrs(time);
  c_pout = pymatrix_to_Carrayptrs(pout);

  //  printf("     > %lf %lf \n  ", theta_antisun/pi*180., theta_boresight/pi*180.);
  double xi, yi, zi, rel_phi;
  double xo, yo, zo;
  double xii, yii, zii;
  double fpout_psi_theta, fpout_psi_phi;
  double omega_pre_t, omega_spin_t;
  //  double phi_earthorb; // defined only to compare with the Guillaume and my python local code

  for(i=0;i<num;i++){

    // This is the anti-sun vector defined by the sun position
    xi = sin(c_theta_asp[i])*cos(c_phi_asp[i]);
    yi = sin(c_theta_asp[i])*sin(c_phi_asp[i]);
    zi = cos(c_theta_asp[i]);

    // this is the temporary offset to compare with the Guillaume's code
    double phi_prec_init_phase = 3./2.*M_PI;
    double phi_spin_init_phase = M_PI;
    //    double phi_earth_init_phase = 3./2.*M_PI;

    //    xi = 0.; //cos(phi_earthorb);
    //    yi = 0.; //sin(phi_earthorb);
    //    zi = 1.; //0.;

    if (yi >= 0) rel_phi = acos(xi);
    if (yi < 0) rel_phi = -acos(xi) + 2.*M_PI;
    //    rel_phi = 2.*M_PI/(3600.*24.*365.) * c_time[i] + phi_earth_init_phase;
    //    printf("%lf %lf %lf \n", omega_pre, omega_spin, c_time[i]);
    omega_pre_t = omega_pre*c_time[i] + phi_prec_init_phase; 
    omega_spin_t = omega_spin*c_time[i] + phi_spin_init_phase; 

    matrix2x2_multi_xy( &xi, &yi,  -rel_phi); // 1->2
    matrix2x2_multi_xz( &xi, &zi,  -pi/2.); // 2->3
    matrix2x2_multi_xz( &xi, &zi, theta_boresight); // 3->4A
    matrix2x2_multi_xy( &xi, &yi, omega_spin_t); // 4A->4B

    // pass the variable to calcualte the psi angle: psi is defined as the phi direction when the focal plane is rotating about the spin axis
    xii = xi;
    yii = yi;
    zii = zi;

    // complete the rest of the boresight pointing calculation
    matrix2x2_multi_xz( &xi, &zi, theta_antisun); // 4B->8
    matrix2x2_multi_xy( &xi, &yi, omega_pre_t); // 8->9
    matrix2x2_multi_xz( &xi, &zi, pi/2.); // 9->10
    matrix2x2_multi_xy( &xi, &yi, rel_phi); // 10->11

    c_pout[0][i] = xi;
    c_pout[1][i] = yi;
    c_pout[2][i] = zi;

    // pick up the remaining calculation for psi
    deriv_phi(xii,yii,zii,&xii,&yii,&zii);
    matrix2x2_multi_xz( &xii, &zii, theta_antisun); // 4B->8
    matrix2x2_multi_xy( &xii, &yii, omega_pre_t); // 8->9
    matrix2x2_multi_xz( &xii, &zii, pi/2.); // 9->10
    matrix2x2_multi_xy( &xii, &yii, rel_phi); // 10->11

    deriv_theta(xi,yi,zi, &xo, &yo, &zo);
    fpout_psi_theta = cosangle(xo,yo,zo,xii,yii,zii);

    deriv_phi(xi,yi,zi, &xo, &yo, &zo);
    fpout_psi_phi = cosangle(xo,yo,zo,xii,yii,zii);

    if (((fpout_psi_theta>0.) & (fpout_psi_theta<=1./2.*pi)) & ((fpout_psi_phi>1./2.*pi) & (fpout_psi_phi<=pi)))
      {
	fpout_psi_theta = - fpout_psi_theta;
      }
    if (((fpout_psi_theta>1./2.*pi) & (fpout_psi_theta<=pi)) & ((fpout_psi_phi>1./2.*pi) & (fpout_psi_phi<=pi)))
      {
	fpout_psi_theta = - fpout_psi_theta;
      }

    c_pout[3][i] = fpout_psi_theta;
  }

  return PyArray_Return(pout);
}
//##############################################################################################
static PyObject *LB_rotmatrix_multi_ellip(PyObject *self, PyObject *args)
{
  /* basic variables */
  long i;
  int num, dims[2];
  double pi=M_PI;
  //  double pi=4.*atan(1.), radeg=(180./pi);
  
  /* variables for inputs */
  PyArrayObject *theta_asp, *phi_asp; // theta and phi of anti-sun vector
  PyArrayObject *phi_antisun, *phi_boresight; // precession and spin axes of satellite rotation
  PyArrayObject *theta_antisun; 
  double theta_boresight;

  /* variable for outputs */
  PyArrayObject *pout;
  
  /* Parse tuples separately since args will differ between C fcns */
  if (!PyArg_ParseTuple(args, "O!O!O!O!dO!", 
			&PyArray_Type, &theta_asp, &PyArray_Type, &phi_asp,
			&PyArray_Type, &theta_antisun, &PyArray_Type, &phi_antisun,
			&theta_boresight, &PyArray_Type, &phi_boresight))  return NULL;
  if (NULL == theta_asp)  return NULL;
  if (NULL == phi_asp)  return NULL;
  if (NULL == phi_antisun)  return NULL;
  if (NULL == phi_boresight)  return NULL;
  
  /* Check that objects are 'double' type and vectors
     Not needed if python wrapper function checks before call to this routine */
  if (not_doublevector(theta_asp)) return NULL;
  if (not_doublevector(phi_asp)) return NULL;
  if (not_doublevector(phi_antisun)) return NULL;
  if (not_doublevector(phi_boresight)) return NULL;
  
  /* Get vector dimension. */
  dims[0] = 3;
  num=dims[1]=theta_asp->dimensions[0];
  pout=(PyArrayObject *) PyArray_FromDims(2,dims,NPY_DOUBLE);

  /* Change contiguous arrays into C * arrays   */
  double *c_theta_asp, *c_phi_asp;
  double *c_phi_antisun, *c_phi_boresight; // precession axis, spin axis
  double *c_theta_antisun;
  double **c_pout;
  c_theta_asp = pyvector_to_Carrayptrs(theta_asp);
  c_phi_asp = pyvector_to_Carrayptrs(phi_asp);

  c_phi_antisun = pyvector_to_Carrayptrs(phi_antisun);
  c_phi_boresight = pyvector_to_Carrayptrs(phi_boresight);
  c_theta_antisun = pyvector_to_Carrayptrs(theta_antisun);
  c_pout = pymatrix_to_Carrayptrs(pout);

  //  printf("     > %lf %lf \n  ", theta_antisun/pi*180., theta_boresight/pi*180.);
  double xi, yi, zi, rel_phi;
  for(i=0;i<num;i++){
    
    xi = sin(c_theta_asp[i])*cos(c_phi_asp[i]);
    yi = sin(c_theta_asp[i])*sin(c_phi_asp[i]);
    zi = cos(c_theta_asp[i]);
    
    if (yi >= 0) rel_phi = acos(xi);
    if (yi < 0) rel_phi = -acos(xi) + 2.*pi;
    matrix2x2_multi_xy( &xi, &yi,  rel_phi);
    matrix2x2_multi_xz( &xi, &zi,  pi/2.);
    matrix2x2_multi_xz( &xi, &zi, -theta_boresight);
    matrix2x2_multi_xy( &xi, &yi, -c_phi_boresight[i]);
    matrix2x2_multi_xz( &xi, &zi, -c_theta_antisun[i]);
    matrix2x2_multi_xy( &xi, &yi, -c_phi_antisun[i]);
    matrix2x2_multi_xz( &xi, &zi, -pi/2.);
    matrix2x2_multi_xy( &xi, &yi, -rel_phi);
    
    c_pout[0][i] = xi;
    c_pout[1][i] = yi;
    c_pout[2][i] = zi;
  }
  
//  free_Carrayptrs(c_theta_asp);
//  free_Carrayptrs(c_phi_asp);
//  free_Carrayptrs(c_phi_antisun);
//  free_Carrayptrs(c_phi_boresight);
  //  free_Carrayptrs(c_pout);
  //  printf("=> %lf %lf %lf \n", c_pout[0][200], c_pout[1][200], c_pout[2][200]);
  //  return Py_BuildValue("i", 1);
  return PyArray_Return(pout);
}
//##############################################################################################

static PyObject *Maps_summingup(PyObject *self, PyObject *args)
{
  /* basic variables */
  int i;
  int dims[2];
  
  /* variables for inputs */
  int nbPix;
  PyArrayObject *ipix, *alpha; // theta and phi of anti-sun vector
  
  /* variable for outputs */
  PyArrayObject *mapout;
  
  /* Parse tuples separately since args will differ between C fcns */
  if (!PyArg_ParseTuple(args, "iO!O!", 
			&nbPix, 
			&PyArray_Type, &ipix, &PyArray_Type, &alpha))  return NULL;
  if (NULL == ipix)  return NULL;
  if (NULL == alpha)  return NULL;
  if (not_doublevector(ipix)) return NULL;
  if (not_doublevector(alpha)) return NULL;
  
  /* Get vector dimension. */
  dims[0] = 7;
  dims[1] = nbPix;
  mapout=(PyArrayObject *) PyArray_FromDims(2,dims,NPY_DOUBLE);
  int nbTOD = alpha->dimensions[0]; 

  /* Change contiguous arrays into C * arrays   */
  double *c_ipix, *c_alpha;
  double **c_mapout;

  c_ipix = pyvector_to_Carrayptrs(ipix);
  c_alpha = pyvector_to_Carrayptrs(alpha);
  c_mapout = pymatrix_to_Carrayptrs(mapout);

  for(i=0;i<nbTOD;i++){
    c_mapout[0][(int)c_ipix[i]] += 1.;
    c_mapout[1][(int)c_ipix[i]] += cos(1.*c_alpha[i]);
    c_mapout[2][(int)c_ipix[i]] += sin(1.*c_alpha[i]);
    c_mapout[3][(int)c_ipix[i]] += cos(2.*c_alpha[i]);
    c_mapout[4][(int)c_ipix[i]] += sin(2.*c_alpha[i]);
    c_mapout[5][(int)c_ipix[i]] += cos(4.*c_alpha[i]);
    c_mapout[6][(int)c_ipix[i]] += sin(4.*c_alpha[i]);
  }
  
  //  printf("ok5\n");
  //    free((char*) c_alpha);
  //    free((char*) c_ipix);
  //  free((char**) c_mapout);
  //  free_Carrayptrs(c_alpha);
  //  free_Carrayptrs(c_ipix);
  //  free_Cint2Darrayptrs(c_mapout);
  //  printf("=> %lf %lf %lf \n", c_pout[0][200], c_pout[1][200], c_pout[2][200]);
  //  return Py_BuildValue("i", 1);
  return PyArray_Return(mapout);
}

//##############################################################################################

static PyMethodDef methods[] = {
  {"LB_rotmatrix_multi", LB_rotmatrix_multi, METH_VARARGS, ""},
  {"LB_rotmatrix_multi2", LB_rotmatrix_multi2, METH_VARARGS, ""},
  {"LB_rotmatrix_multi_ellip", LB_rotmatrix_multi_ellip, METH_VARARGS, ""},
  {"Maps_summingup", Maps_summingup, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
};

// init(modulename)
PyMODINIT_FUNC initlib_LBScan_c(void)
{
  (void)Py_InitModule("lib_LBScan_c", methods);  // "modulename"
  import_array();
}
