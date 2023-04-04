#include "gnss.h"

namespace {
#define DIM 11
#define EDIM 11
#define MEDIM 11
typedef void (*Hfun)(double *, double *, double *);
const static double MAHA_THRESH_6 = 3.8414588206941227;
const static double MAHA_THRESH_20 = 3.8414588206941227;
const static double MAHA_THRESH_7 = 3.8414588206941227;
const static double MAHA_THRESH_21 = 3.8414588206941227;

/******************************************************************************
 *                      Code generated with SymPy 1.11.1                      *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_794957986474022410) {
   out_794957986474022410[0] = delta_x[0] + nom_x[0];
   out_794957986474022410[1] = delta_x[1] + nom_x[1];
   out_794957986474022410[2] = delta_x[2] + nom_x[2];
   out_794957986474022410[3] = delta_x[3] + nom_x[3];
   out_794957986474022410[4] = delta_x[4] + nom_x[4];
   out_794957986474022410[5] = delta_x[5] + nom_x[5];
   out_794957986474022410[6] = delta_x[6] + nom_x[6];
   out_794957986474022410[7] = delta_x[7] + nom_x[7];
   out_794957986474022410[8] = delta_x[8] + nom_x[8];
   out_794957986474022410[9] = delta_x[9] + nom_x[9];
   out_794957986474022410[10] = delta_x[10] + nom_x[10];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_7380037827036718422) {
   out_7380037827036718422[0] = -nom_x[0] + true_x[0];
   out_7380037827036718422[1] = -nom_x[1] + true_x[1];
   out_7380037827036718422[2] = -nom_x[2] + true_x[2];
   out_7380037827036718422[3] = -nom_x[3] + true_x[3];
   out_7380037827036718422[4] = -nom_x[4] + true_x[4];
   out_7380037827036718422[5] = -nom_x[5] + true_x[5];
   out_7380037827036718422[6] = -nom_x[6] + true_x[6];
   out_7380037827036718422[7] = -nom_x[7] + true_x[7];
   out_7380037827036718422[8] = -nom_x[8] + true_x[8];
   out_7380037827036718422[9] = -nom_x[9] + true_x[9];
   out_7380037827036718422[10] = -nom_x[10] + true_x[10];
}
void H_mod_fun(double *state, double *out_3537326562386189174) {
   out_3537326562386189174[0] = 1.0;
   out_3537326562386189174[1] = 0;
   out_3537326562386189174[2] = 0;
   out_3537326562386189174[3] = 0;
   out_3537326562386189174[4] = 0;
   out_3537326562386189174[5] = 0;
   out_3537326562386189174[6] = 0;
   out_3537326562386189174[7] = 0;
   out_3537326562386189174[8] = 0;
   out_3537326562386189174[9] = 0;
   out_3537326562386189174[10] = 0;
   out_3537326562386189174[11] = 0;
   out_3537326562386189174[12] = 1.0;
   out_3537326562386189174[13] = 0;
   out_3537326562386189174[14] = 0;
   out_3537326562386189174[15] = 0;
   out_3537326562386189174[16] = 0;
   out_3537326562386189174[17] = 0;
   out_3537326562386189174[18] = 0;
   out_3537326562386189174[19] = 0;
   out_3537326562386189174[20] = 0;
   out_3537326562386189174[21] = 0;
   out_3537326562386189174[22] = 0;
   out_3537326562386189174[23] = 0;
   out_3537326562386189174[24] = 1.0;
   out_3537326562386189174[25] = 0;
   out_3537326562386189174[26] = 0;
   out_3537326562386189174[27] = 0;
   out_3537326562386189174[28] = 0;
   out_3537326562386189174[29] = 0;
   out_3537326562386189174[30] = 0;
   out_3537326562386189174[31] = 0;
   out_3537326562386189174[32] = 0;
   out_3537326562386189174[33] = 0;
   out_3537326562386189174[34] = 0;
   out_3537326562386189174[35] = 0;
   out_3537326562386189174[36] = 1.0;
   out_3537326562386189174[37] = 0;
   out_3537326562386189174[38] = 0;
   out_3537326562386189174[39] = 0;
   out_3537326562386189174[40] = 0;
   out_3537326562386189174[41] = 0;
   out_3537326562386189174[42] = 0;
   out_3537326562386189174[43] = 0;
   out_3537326562386189174[44] = 0;
   out_3537326562386189174[45] = 0;
   out_3537326562386189174[46] = 0;
   out_3537326562386189174[47] = 0;
   out_3537326562386189174[48] = 1.0;
   out_3537326562386189174[49] = 0;
   out_3537326562386189174[50] = 0;
   out_3537326562386189174[51] = 0;
   out_3537326562386189174[52] = 0;
   out_3537326562386189174[53] = 0;
   out_3537326562386189174[54] = 0;
   out_3537326562386189174[55] = 0;
   out_3537326562386189174[56] = 0;
   out_3537326562386189174[57] = 0;
   out_3537326562386189174[58] = 0;
   out_3537326562386189174[59] = 0;
   out_3537326562386189174[60] = 1.0;
   out_3537326562386189174[61] = 0;
   out_3537326562386189174[62] = 0;
   out_3537326562386189174[63] = 0;
   out_3537326562386189174[64] = 0;
   out_3537326562386189174[65] = 0;
   out_3537326562386189174[66] = 0;
   out_3537326562386189174[67] = 0;
   out_3537326562386189174[68] = 0;
   out_3537326562386189174[69] = 0;
   out_3537326562386189174[70] = 0;
   out_3537326562386189174[71] = 0;
   out_3537326562386189174[72] = 1.0;
   out_3537326562386189174[73] = 0;
   out_3537326562386189174[74] = 0;
   out_3537326562386189174[75] = 0;
   out_3537326562386189174[76] = 0;
   out_3537326562386189174[77] = 0;
   out_3537326562386189174[78] = 0;
   out_3537326562386189174[79] = 0;
   out_3537326562386189174[80] = 0;
   out_3537326562386189174[81] = 0;
   out_3537326562386189174[82] = 0;
   out_3537326562386189174[83] = 0;
   out_3537326562386189174[84] = 1.0;
   out_3537326562386189174[85] = 0;
   out_3537326562386189174[86] = 0;
   out_3537326562386189174[87] = 0;
   out_3537326562386189174[88] = 0;
   out_3537326562386189174[89] = 0;
   out_3537326562386189174[90] = 0;
   out_3537326562386189174[91] = 0;
   out_3537326562386189174[92] = 0;
   out_3537326562386189174[93] = 0;
   out_3537326562386189174[94] = 0;
   out_3537326562386189174[95] = 0;
   out_3537326562386189174[96] = 1.0;
   out_3537326562386189174[97] = 0;
   out_3537326562386189174[98] = 0;
   out_3537326562386189174[99] = 0;
   out_3537326562386189174[100] = 0;
   out_3537326562386189174[101] = 0;
   out_3537326562386189174[102] = 0;
   out_3537326562386189174[103] = 0;
   out_3537326562386189174[104] = 0;
   out_3537326562386189174[105] = 0;
   out_3537326562386189174[106] = 0;
   out_3537326562386189174[107] = 0;
   out_3537326562386189174[108] = 1.0;
   out_3537326562386189174[109] = 0;
   out_3537326562386189174[110] = 0;
   out_3537326562386189174[111] = 0;
   out_3537326562386189174[112] = 0;
   out_3537326562386189174[113] = 0;
   out_3537326562386189174[114] = 0;
   out_3537326562386189174[115] = 0;
   out_3537326562386189174[116] = 0;
   out_3537326562386189174[117] = 0;
   out_3537326562386189174[118] = 0;
   out_3537326562386189174[119] = 0;
   out_3537326562386189174[120] = 1.0;
}
void f_fun(double *state, double dt, double *out_3201302353069732401) {
   out_3201302353069732401[0] = dt*state[3] + state[0];
   out_3201302353069732401[1] = dt*state[4] + state[1];
   out_3201302353069732401[2] = dt*state[5] + state[2];
   out_3201302353069732401[3] = state[3];
   out_3201302353069732401[4] = state[4];
   out_3201302353069732401[5] = state[5];
   out_3201302353069732401[6] = dt*state[7] + state[6];
   out_3201302353069732401[7] = dt*state[8] + state[7];
   out_3201302353069732401[8] = state[8];
   out_3201302353069732401[9] = state[9];
   out_3201302353069732401[10] = state[10];
}
void F_fun(double *state, double dt, double *out_4949750205820914524) {
   out_4949750205820914524[0] = 1;
   out_4949750205820914524[1] = 0;
   out_4949750205820914524[2] = 0;
   out_4949750205820914524[3] = dt;
   out_4949750205820914524[4] = 0;
   out_4949750205820914524[5] = 0;
   out_4949750205820914524[6] = 0;
   out_4949750205820914524[7] = 0;
   out_4949750205820914524[8] = 0;
   out_4949750205820914524[9] = 0;
   out_4949750205820914524[10] = 0;
   out_4949750205820914524[11] = 0;
   out_4949750205820914524[12] = 1;
   out_4949750205820914524[13] = 0;
   out_4949750205820914524[14] = 0;
   out_4949750205820914524[15] = dt;
   out_4949750205820914524[16] = 0;
   out_4949750205820914524[17] = 0;
   out_4949750205820914524[18] = 0;
   out_4949750205820914524[19] = 0;
   out_4949750205820914524[20] = 0;
   out_4949750205820914524[21] = 0;
   out_4949750205820914524[22] = 0;
   out_4949750205820914524[23] = 0;
   out_4949750205820914524[24] = 1;
   out_4949750205820914524[25] = 0;
   out_4949750205820914524[26] = 0;
   out_4949750205820914524[27] = dt;
   out_4949750205820914524[28] = 0;
   out_4949750205820914524[29] = 0;
   out_4949750205820914524[30] = 0;
   out_4949750205820914524[31] = 0;
   out_4949750205820914524[32] = 0;
   out_4949750205820914524[33] = 0;
   out_4949750205820914524[34] = 0;
   out_4949750205820914524[35] = 0;
   out_4949750205820914524[36] = 1;
   out_4949750205820914524[37] = 0;
   out_4949750205820914524[38] = 0;
   out_4949750205820914524[39] = 0;
   out_4949750205820914524[40] = 0;
   out_4949750205820914524[41] = 0;
   out_4949750205820914524[42] = 0;
   out_4949750205820914524[43] = 0;
   out_4949750205820914524[44] = 0;
   out_4949750205820914524[45] = 0;
   out_4949750205820914524[46] = 0;
   out_4949750205820914524[47] = 0;
   out_4949750205820914524[48] = 1;
   out_4949750205820914524[49] = 0;
   out_4949750205820914524[50] = 0;
   out_4949750205820914524[51] = 0;
   out_4949750205820914524[52] = 0;
   out_4949750205820914524[53] = 0;
   out_4949750205820914524[54] = 0;
   out_4949750205820914524[55] = 0;
   out_4949750205820914524[56] = 0;
   out_4949750205820914524[57] = 0;
   out_4949750205820914524[58] = 0;
   out_4949750205820914524[59] = 0;
   out_4949750205820914524[60] = 1;
   out_4949750205820914524[61] = 0;
   out_4949750205820914524[62] = 0;
   out_4949750205820914524[63] = 0;
   out_4949750205820914524[64] = 0;
   out_4949750205820914524[65] = 0;
   out_4949750205820914524[66] = 0;
   out_4949750205820914524[67] = 0;
   out_4949750205820914524[68] = 0;
   out_4949750205820914524[69] = 0;
   out_4949750205820914524[70] = 0;
   out_4949750205820914524[71] = 0;
   out_4949750205820914524[72] = 1;
   out_4949750205820914524[73] = dt;
   out_4949750205820914524[74] = 0;
   out_4949750205820914524[75] = 0;
   out_4949750205820914524[76] = 0;
   out_4949750205820914524[77] = 0;
   out_4949750205820914524[78] = 0;
   out_4949750205820914524[79] = 0;
   out_4949750205820914524[80] = 0;
   out_4949750205820914524[81] = 0;
   out_4949750205820914524[82] = 0;
   out_4949750205820914524[83] = 0;
   out_4949750205820914524[84] = 1;
   out_4949750205820914524[85] = dt;
   out_4949750205820914524[86] = 0;
   out_4949750205820914524[87] = 0;
   out_4949750205820914524[88] = 0;
   out_4949750205820914524[89] = 0;
   out_4949750205820914524[90] = 0;
   out_4949750205820914524[91] = 0;
   out_4949750205820914524[92] = 0;
   out_4949750205820914524[93] = 0;
   out_4949750205820914524[94] = 0;
   out_4949750205820914524[95] = 0;
   out_4949750205820914524[96] = 1;
   out_4949750205820914524[97] = 0;
   out_4949750205820914524[98] = 0;
   out_4949750205820914524[99] = 0;
   out_4949750205820914524[100] = 0;
   out_4949750205820914524[101] = 0;
   out_4949750205820914524[102] = 0;
   out_4949750205820914524[103] = 0;
   out_4949750205820914524[104] = 0;
   out_4949750205820914524[105] = 0;
   out_4949750205820914524[106] = 0;
   out_4949750205820914524[107] = 0;
   out_4949750205820914524[108] = 1;
   out_4949750205820914524[109] = 0;
   out_4949750205820914524[110] = 0;
   out_4949750205820914524[111] = 0;
   out_4949750205820914524[112] = 0;
   out_4949750205820914524[113] = 0;
   out_4949750205820914524[114] = 0;
   out_4949750205820914524[115] = 0;
   out_4949750205820914524[116] = 0;
   out_4949750205820914524[117] = 0;
   out_4949750205820914524[118] = 0;
   out_4949750205820914524[119] = 0;
   out_4949750205820914524[120] = 1;
}
void h_6(double *state, double *sat_pos, double *out_4222654111953870605) {
   out_4222654111953870605[0] = sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2)) + state[6];
}
void H_6(double *state, double *sat_pos, double *out_1907527026073269766) {
   out_1907527026073269766[0] = (-sat_pos[0] + state[0])/sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2));
   out_1907527026073269766[1] = (-sat_pos[1] + state[1])/sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2));
   out_1907527026073269766[2] = (-sat_pos[2] + state[2])/sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2));
   out_1907527026073269766[3] = 0;
   out_1907527026073269766[4] = 0;
   out_1907527026073269766[5] = 0;
   out_1907527026073269766[6] = 1;
   out_1907527026073269766[7] = 0;
   out_1907527026073269766[8] = 0;
   out_1907527026073269766[9] = 0;
   out_1907527026073269766[10] = 0;
}
void h_20(double *state, double *sat_pos, double *out_7050938256011114664) {
   out_7050938256011114664[0] = sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2)) + sat_pos[3]*state[10] + state[6] + state[9];
}
void H_20(double *state, double *sat_pos, double *out_6133602518692230455) {
   out_6133602518692230455[0] = (-sat_pos[0] + state[0])/sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2));
   out_6133602518692230455[1] = (-sat_pos[1] + state[1])/sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2));
   out_6133602518692230455[2] = (-sat_pos[2] + state[2])/sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2));
   out_6133602518692230455[3] = 0;
   out_6133602518692230455[4] = 0;
   out_6133602518692230455[5] = 0;
   out_6133602518692230455[6] = 1;
   out_6133602518692230455[7] = 0;
   out_6133602518692230455[8] = 0;
   out_6133602518692230455[9] = 1;
   out_6133602518692230455[10] = sat_pos[3];
}
void h_7(double *state, double *sat_pos_vel, double *out_6212552211602838251) {
   out_6212552211602838251[0] = (sat_pos_vel[0] - state[0])*(sat_pos_vel[3] - state[3])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2)) + (sat_pos_vel[1] - state[1])*(sat_pos_vel[4] - state[4])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2)) + (sat_pos_vel[2] - state[2])*(sat_pos_vel[5] - state[5])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2)) + state[7];
}
void H_7(double *state, double *sat_pos_vel, double *out_2598800062447436765) {
   out_2598800062447436765[0] = pow(sat_pos_vel[0] - state[0], 2)*(sat_pos_vel[3] - state[3])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[0] - state[0])*(sat_pos_vel[1] - state[1])*(sat_pos_vel[4] - state[4])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[0] - state[0])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[5] - state[5])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) - (sat_pos_vel[3] - state[3])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_2598800062447436765[1] = (sat_pos_vel[0] - state[0])*(sat_pos_vel[1] - state[1])*(sat_pos_vel[3] - state[3])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + pow(sat_pos_vel[1] - state[1], 2)*(sat_pos_vel[4] - state[4])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[1] - state[1])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[5] - state[5])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) - (sat_pos_vel[4] - state[4])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_2598800062447436765[2] = (sat_pos_vel[0] - state[0])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[3] - state[3])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[1] - state[1])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[4] - state[4])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + pow(sat_pos_vel[2] - state[2], 2)*(sat_pos_vel[5] - state[5])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) - (sat_pos_vel[5] - state[5])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_2598800062447436765[3] = -(sat_pos_vel[0] - state[0])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_2598800062447436765[4] = -(sat_pos_vel[1] - state[1])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_2598800062447436765[5] = -(sat_pos_vel[2] - state[2])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_2598800062447436765[6] = 0;
   out_2598800062447436765[7] = 1;
   out_2598800062447436765[8] = 0;
   out_2598800062447436765[9] = 0;
   out_2598800062447436765[10] = 0;
}
void h_21(double *state, double *sat_pos_vel, double *out_6212552211602838251) {
   out_6212552211602838251[0] = (sat_pos_vel[0] - state[0])*(sat_pos_vel[3] - state[3])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2)) + (sat_pos_vel[1] - state[1])*(sat_pos_vel[4] - state[4])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2)) + (sat_pos_vel[2] - state[2])*(sat_pos_vel[5] - state[5])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2)) + state[7];
}
void H_21(double *state, double *sat_pos_vel, double *out_2598800062447436765) {
   out_2598800062447436765[0] = pow(sat_pos_vel[0] - state[0], 2)*(sat_pos_vel[3] - state[3])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[0] - state[0])*(sat_pos_vel[1] - state[1])*(sat_pos_vel[4] - state[4])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[0] - state[0])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[5] - state[5])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) - (sat_pos_vel[3] - state[3])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_2598800062447436765[1] = (sat_pos_vel[0] - state[0])*(sat_pos_vel[1] - state[1])*(sat_pos_vel[3] - state[3])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + pow(sat_pos_vel[1] - state[1], 2)*(sat_pos_vel[4] - state[4])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[1] - state[1])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[5] - state[5])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) - (sat_pos_vel[4] - state[4])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_2598800062447436765[2] = (sat_pos_vel[0] - state[0])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[3] - state[3])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[1] - state[1])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[4] - state[4])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + pow(sat_pos_vel[2] - state[2], 2)*(sat_pos_vel[5] - state[5])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) - (sat_pos_vel[5] - state[5])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_2598800062447436765[3] = -(sat_pos_vel[0] - state[0])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_2598800062447436765[4] = -(sat_pos_vel[1] - state[1])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_2598800062447436765[5] = -(sat_pos_vel[2] - state[2])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_2598800062447436765[6] = 0;
   out_2598800062447436765[7] = 1;
   out_2598800062447436765[8] = 0;
   out_2598800062447436765[9] = 0;
   out_2598800062447436765[10] = 0;
}
#include <eigen3/Eigen/Dense>
#include <iostream>

typedef Eigen::Matrix<double, DIM, DIM, Eigen::RowMajor> DDM;
typedef Eigen::Matrix<double, EDIM, EDIM, Eigen::RowMajor> EEM;
typedef Eigen::Matrix<double, DIM, EDIM, Eigen::RowMajor> DEM;

void predict(double *in_x, double *in_P, double *in_Q, double dt) {
  typedef Eigen::Matrix<double, MEDIM, MEDIM, Eigen::RowMajor> RRM;

  double nx[DIM] = {0};
  double in_F[EDIM*EDIM] = {0};

  // functions from sympy
  f_fun(in_x, dt, nx);
  F_fun(in_x, dt, in_F);


  EEM F(in_F);
  EEM P(in_P);
  EEM Q(in_Q);

  RRM F_main = F.topLeftCorner(MEDIM, MEDIM);
  P.topLeftCorner(MEDIM, MEDIM) = (F_main * P.topLeftCorner(MEDIM, MEDIM)) * F_main.transpose();
  P.topRightCorner(MEDIM, EDIM - MEDIM) = F_main * P.topRightCorner(MEDIM, EDIM - MEDIM);
  P.bottomLeftCorner(EDIM - MEDIM, MEDIM) = P.bottomLeftCorner(EDIM - MEDIM, MEDIM) * F_main.transpose();

  P = P + dt*Q;

  // copy out state
  memcpy(in_x, nx, DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
}

// note: extra_args dim only correct when null space projecting
// otherwise 1
template <int ZDIM, int EADIM, bool MAHA_TEST>
void update(double *in_x, double *in_P, Hfun h_fun, Hfun H_fun, Hfun Hea_fun, double *in_z, double *in_R, double *in_ea, double MAHA_THRESHOLD) {
  typedef Eigen::Matrix<double, ZDIM, ZDIM, Eigen::RowMajor> ZZM;
  typedef Eigen::Matrix<double, ZDIM, DIM, Eigen::RowMajor> ZDM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, EDIM, Eigen::RowMajor> XEM;
  //typedef Eigen::Matrix<double, EDIM, ZDIM, Eigen::RowMajor> EZM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> X1M;
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> XXM;

  double in_hx[ZDIM] = {0};
  double in_H[ZDIM * DIM] = {0};
  double in_H_mod[EDIM * DIM] = {0};
  double delta_x[EDIM] = {0};
  double x_new[DIM] = {0};


  // state x, P
  Eigen::Matrix<double, ZDIM, 1> z(in_z);
  EEM P(in_P);
  ZZM pre_R(in_R);

  // functions from sympy
  h_fun(in_x, in_ea, in_hx);
  H_fun(in_x, in_ea, in_H);
  ZDM pre_H(in_H);

  // get y (y = z - hx)
  Eigen::Matrix<double, ZDIM, 1> pre_y(in_hx); pre_y = z - pre_y;
  X1M y; XXM H; XXM R;
  if (Hea_fun){
    typedef Eigen::Matrix<double, ZDIM, EADIM, Eigen::RowMajor> ZAM;
    double in_Hea[ZDIM * EADIM] = {0};
    Hea_fun(in_x, in_ea, in_Hea);
    ZAM Hea(in_Hea);
    XXM A = Hea.transpose().fullPivLu().kernel();


    y = A.transpose() * pre_y;
    H = A.transpose() * pre_H;
    R = A.transpose() * pre_R * A;
  } else {
    y = pre_y;
    H = pre_H;
    R = pre_R;
  }
  // get modified H
  H_mod_fun(in_x, in_H_mod);
  DEM H_mod(in_H_mod);
  XEM H_err = H * H_mod;

  // Do mahalobis distance test
  if (MAHA_TEST){
    XXM a = (H_err * P * H_err.transpose() + R).inverse();
    double maha_dist = y.transpose() * a * y;
    if (maha_dist > MAHA_THRESHOLD){
      R = 1.0e16 * R;
    }
  }

  // Outlier resilient weighting
  double weight = 1;//(1.5)/(1 + y.squaredNorm()/R.sum());

  // kalman gains and I_KH
  XXM S = ((H_err * P) * H_err.transpose()) + R/weight;
  XEM KT = S.fullPivLu().solve(H_err * P.transpose());
  //EZM K = KT.transpose(); TODO: WHY DOES THIS NOT COMPILE?
  //EZM K = S.fullPivLu().solve(H_err * P.transpose()).transpose();
  //std::cout << "Here is the matrix rot:\n" << K << std::endl;
  EEM I_KH = Eigen::Matrix<double, EDIM, EDIM>::Identity() - (KT.transpose() * H_err);

  // update state by injecting dx
  Eigen::Matrix<double, EDIM, 1> dx(delta_x);
  dx  = (KT.transpose() * y);
  memcpy(delta_x, dx.data(), EDIM * sizeof(double));
  err_fun(in_x, delta_x, x_new);
  Eigen::Matrix<double, DIM, 1> x(x_new);

  // update cov
  P = ((I_KH * P) * I_KH.transpose()) + ((KT.transpose() * R) * KT);

  // copy out state
  memcpy(in_x, x.data(), DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
  memcpy(in_z, y.data(), y.rows() * sizeof(double));
}




}
extern "C" {

void gnss_update_6(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_6, H_6, NULL, in_z, in_R, in_ea, MAHA_THRESH_6);
}
void gnss_update_20(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_20, H_20, NULL, in_z, in_R, in_ea, MAHA_THRESH_20);
}
void gnss_update_7(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_7, H_7, NULL, in_z, in_R, in_ea, MAHA_THRESH_7);
}
void gnss_update_21(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_21, H_21, NULL, in_z, in_R, in_ea, MAHA_THRESH_21);
}
void gnss_err_fun(double *nom_x, double *delta_x, double *out_794957986474022410) {
  err_fun(nom_x, delta_x, out_794957986474022410);
}
void gnss_inv_err_fun(double *nom_x, double *true_x, double *out_7380037827036718422) {
  inv_err_fun(nom_x, true_x, out_7380037827036718422);
}
void gnss_H_mod_fun(double *state, double *out_3537326562386189174) {
  H_mod_fun(state, out_3537326562386189174);
}
void gnss_f_fun(double *state, double dt, double *out_3201302353069732401) {
  f_fun(state,  dt, out_3201302353069732401);
}
void gnss_F_fun(double *state, double dt, double *out_4949750205820914524) {
  F_fun(state,  dt, out_4949750205820914524);
}
void gnss_h_6(double *state, double *sat_pos, double *out_4222654111953870605) {
  h_6(state, sat_pos, out_4222654111953870605);
}
void gnss_H_6(double *state, double *sat_pos, double *out_1907527026073269766) {
  H_6(state, sat_pos, out_1907527026073269766);
}
void gnss_h_20(double *state, double *sat_pos, double *out_7050938256011114664) {
  h_20(state, sat_pos, out_7050938256011114664);
}
void gnss_H_20(double *state, double *sat_pos, double *out_6133602518692230455) {
  H_20(state, sat_pos, out_6133602518692230455);
}
void gnss_h_7(double *state, double *sat_pos_vel, double *out_6212552211602838251) {
  h_7(state, sat_pos_vel, out_6212552211602838251);
}
void gnss_H_7(double *state, double *sat_pos_vel, double *out_2598800062447436765) {
  H_7(state, sat_pos_vel, out_2598800062447436765);
}
void gnss_h_21(double *state, double *sat_pos_vel, double *out_6212552211602838251) {
  h_21(state, sat_pos_vel, out_6212552211602838251);
}
void gnss_H_21(double *state, double *sat_pos_vel, double *out_2598800062447436765) {
  H_21(state, sat_pos_vel, out_2598800062447436765);
}
void gnss_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
}

const EKF gnss = {
  .name = "gnss",
  .kinds = { 6, 20, 7, 21 },
  .feature_kinds = {  },
  .f_fun = gnss_f_fun,
  .F_fun = gnss_F_fun,
  .err_fun = gnss_err_fun,
  .inv_err_fun = gnss_inv_err_fun,
  .H_mod_fun = gnss_H_mod_fun,
  .predict = gnss_predict,
  .hs = {
    { 6, gnss_h_6 },
    { 20, gnss_h_20 },
    { 7, gnss_h_7 },
    { 21, gnss_h_21 },
  },
  .Hs = {
    { 6, gnss_H_6 },
    { 20, gnss_H_20 },
    { 7, gnss_H_7 },
    { 21, gnss_H_21 },
  },
  .updates = {
    { 6, gnss_update_6 },
    { 20, gnss_update_20 },
    { 7, gnss_update_7 },
    { 21, gnss_update_21 },
  },
  .Hes = {
  },
  .sets = {
  },
  .extra_routines = {
  },
};

ekf_init(gnss);
