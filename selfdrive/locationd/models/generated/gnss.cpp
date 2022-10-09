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
void err_fun(double *nom_x, double *delta_x, double *out_155765044166347563) {
   out_155765044166347563[0] = delta_x[0] + nom_x[0];
   out_155765044166347563[1] = delta_x[1] + nom_x[1];
   out_155765044166347563[2] = delta_x[2] + nom_x[2];
   out_155765044166347563[3] = delta_x[3] + nom_x[3];
   out_155765044166347563[4] = delta_x[4] + nom_x[4];
   out_155765044166347563[5] = delta_x[5] + nom_x[5];
   out_155765044166347563[6] = delta_x[6] + nom_x[6];
   out_155765044166347563[7] = delta_x[7] + nom_x[7];
   out_155765044166347563[8] = delta_x[8] + nom_x[8];
   out_155765044166347563[9] = delta_x[9] + nom_x[9];
   out_155765044166347563[10] = delta_x[10] + nom_x[10];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_5147265498242855152) {
   out_5147265498242855152[0] = -nom_x[0] + true_x[0];
   out_5147265498242855152[1] = -nom_x[1] + true_x[1];
   out_5147265498242855152[2] = -nom_x[2] + true_x[2];
   out_5147265498242855152[3] = -nom_x[3] + true_x[3];
   out_5147265498242855152[4] = -nom_x[4] + true_x[4];
   out_5147265498242855152[5] = -nom_x[5] + true_x[5];
   out_5147265498242855152[6] = -nom_x[6] + true_x[6];
   out_5147265498242855152[7] = -nom_x[7] + true_x[7];
   out_5147265498242855152[8] = -nom_x[8] + true_x[8];
   out_5147265498242855152[9] = -nom_x[9] + true_x[9];
   out_5147265498242855152[10] = -nom_x[10] + true_x[10];
}
void H_mod_fun(double *state, double *out_2441002244825976478) {
   out_2441002244825976478[0] = 1.0;
   out_2441002244825976478[1] = 0;
   out_2441002244825976478[2] = 0;
   out_2441002244825976478[3] = 0;
   out_2441002244825976478[4] = 0;
   out_2441002244825976478[5] = 0;
   out_2441002244825976478[6] = 0;
   out_2441002244825976478[7] = 0;
   out_2441002244825976478[8] = 0;
   out_2441002244825976478[9] = 0;
   out_2441002244825976478[10] = 0;
   out_2441002244825976478[11] = 0;
   out_2441002244825976478[12] = 1.0;
   out_2441002244825976478[13] = 0;
   out_2441002244825976478[14] = 0;
   out_2441002244825976478[15] = 0;
   out_2441002244825976478[16] = 0;
   out_2441002244825976478[17] = 0;
   out_2441002244825976478[18] = 0;
   out_2441002244825976478[19] = 0;
   out_2441002244825976478[20] = 0;
   out_2441002244825976478[21] = 0;
   out_2441002244825976478[22] = 0;
   out_2441002244825976478[23] = 0;
   out_2441002244825976478[24] = 1.0;
   out_2441002244825976478[25] = 0;
   out_2441002244825976478[26] = 0;
   out_2441002244825976478[27] = 0;
   out_2441002244825976478[28] = 0;
   out_2441002244825976478[29] = 0;
   out_2441002244825976478[30] = 0;
   out_2441002244825976478[31] = 0;
   out_2441002244825976478[32] = 0;
   out_2441002244825976478[33] = 0;
   out_2441002244825976478[34] = 0;
   out_2441002244825976478[35] = 0;
   out_2441002244825976478[36] = 1.0;
   out_2441002244825976478[37] = 0;
   out_2441002244825976478[38] = 0;
   out_2441002244825976478[39] = 0;
   out_2441002244825976478[40] = 0;
   out_2441002244825976478[41] = 0;
   out_2441002244825976478[42] = 0;
   out_2441002244825976478[43] = 0;
   out_2441002244825976478[44] = 0;
   out_2441002244825976478[45] = 0;
   out_2441002244825976478[46] = 0;
   out_2441002244825976478[47] = 0;
   out_2441002244825976478[48] = 1.0;
   out_2441002244825976478[49] = 0;
   out_2441002244825976478[50] = 0;
   out_2441002244825976478[51] = 0;
   out_2441002244825976478[52] = 0;
   out_2441002244825976478[53] = 0;
   out_2441002244825976478[54] = 0;
   out_2441002244825976478[55] = 0;
   out_2441002244825976478[56] = 0;
   out_2441002244825976478[57] = 0;
   out_2441002244825976478[58] = 0;
   out_2441002244825976478[59] = 0;
   out_2441002244825976478[60] = 1.0;
   out_2441002244825976478[61] = 0;
   out_2441002244825976478[62] = 0;
   out_2441002244825976478[63] = 0;
   out_2441002244825976478[64] = 0;
   out_2441002244825976478[65] = 0;
   out_2441002244825976478[66] = 0;
   out_2441002244825976478[67] = 0;
   out_2441002244825976478[68] = 0;
   out_2441002244825976478[69] = 0;
   out_2441002244825976478[70] = 0;
   out_2441002244825976478[71] = 0;
   out_2441002244825976478[72] = 1.0;
   out_2441002244825976478[73] = 0;
   out_2441002244825976478[74] = 0;
   out_2441002244825976478[75] = 0;
   out_2441002244825976478[76] = 0;
   out_2441002244825976478[77] = 0;
   out_2441002244825976478[78] = 0;
   out_2441002244825976478[79] = 0;
   out_2441002244825976478[80] = 0;
   out_2441002244825976478[81] = 0;
   out_2441002244825976478[82] = 0;
   out_2441002244825976478[83] = 0;
   out_2441002244825976478[84] = 1.0;
   out_2441002244825976478[85] = 0;
   out_2441002244825976478[86] = 0;
   out_2441002244825976478[87] = 0;
   out_2441002244825976478[88] = 0;
   out_2441002244825976478[89] = 0;
   out_2441002244825976478[90] = 0;
   out_2441002244825976478[91] = 0;
   out_2441002244825976478[92] = 0;
   out_2441002244825976478[93] = 0;
   out_2441002244825976478[94] = 0;
   out_2441002244825976478[95] = 0;
   out_2441002244825976478[96] = 1.0;
   out_2441002244825976478[97] = 0;
   out_2441002244825976478[98] = 0;
   out_2441002244825976478[99] = 0;
   out_2441002244825976478[100] = 0;
   out_2441002244825976478[101] = 0;
   out_2441002244825976478[102] = 0;
   out_2441002244825976478[103] = 0;
   out_2441002244825976478[104] = 0;
   out_2441002244825976478[105] = 0;
   out_2441002244825976478[106] = 0;
   out_2441002244825976478[107] = 0;
   out_2441002244825976478[108] = 1.0;
   out_2441002244825976478[109] = 0;
   out_2441002244825976478[110] = 0;
   out_2441002244825976478[111] = 0;
   out_2441002244825976478[112] = 0;
   out_2441002244825976478[113] = 0;
   out_2441002244825976478[114] = 0;
   out_2441002244825976478[115] = 0;
   out_2441002244825976478[116] = 0;
   out_2441002244825976478[117] = 0;
   out_2441002244825976478[118] = 0;
   out_2441002244825976478[119] = 0;
   out_2441002244825976478[120] = 1.0;
}
void f_fun(double *state, double dt, double *out_5834456982122438687) {
   out_5834456982122438687[0] = dt*state[3] + state[0];
   out_5834456982122438687[1] = dt*state[4] + state[1];
   out_5834456982122438687[2] = dt*state[5] + state[2];
   out_5834456982122438687[3] = state[3];
   out_5834456982122438687[4] = state[4];
   out_5834456982122438687[5] = state[5];
   out_5834456982122438687[6] = dt*state[7] + state[6];
   out_5834456982122438687[7] = dt*state[8] + state[7];
   out_5834456982122438687[8] = state[8];
   out_5834456982122438687[9] = state[9];
   out_5834456982122438687[10] = state[10];
}
void F_fun(double *state, double dt, double *out_6970338126122875519) {
   out_6970338126122875519[0] = 1;
   out_6970338126122875519[1] = 0;
   out_6970338126122875519[2] = 0;
   out_6970338126122875519[3] = dt;
   out_6970338126122875519[4] = 0;
   out_6970338126122875519[5] = 0;
   out_6970338126122875519[6] = 0;
   out_6970338126122875519[7] = 0;
   out_6970338126122875519[8] = 0;
   out_6970338126122875519[9] = 0;
   out_6970338126122875519[10] = 0;
   out_6970338126122875519[11] = 0;
   out_6970338126122875519[12] = 1;
   out_6970338126122875519[13] = 0;
   out_6970338126122875519[14] = 0;
   out_6970338126122875519[15] = dt;
   out_6970338126122875519[16] = 0;
   out_6970338126122875519[17] = 0;
   out_6970338126122875519[18] = 0;
   out_6970338126122875519[19] = 0;
   out_6970338126122875519[20] = 0;
   out_6970338126122875519[21] = 0;
   out_6970338126122875519[22] = 0;
   out_6970338126122875519[23] = 0;
   out_6970338126122875519[24] = 1;
   out_6970338126122875519[25] = 0;
   out_6970338126122875519[26] = 0;
   out_6970338126122875519[27] = dt;
   out_6970338126122875519[28] = 0;
   out_6970338126122875519[29] = 0;
   out_6970338126122875519[30] = 0;
   out_6970338126122875519[31] = 0;
   out_6970338126122875519[32] = 0;
   out_6970338126122875519[33] = 0;
   out_6970338126122875519[34] = 0;
   out_6970338126122875519[35] = 0;
   out_6970338126122875519[36] = 1;
   out_6970338126122875519[37] = 0;
   out_6970338126122875519[38] = 0;
   out_6970338126122875519[39] = 0;
   out_6970338126122875519[40] = 0;
   out_6970338126122875519[41] = 0;
   out_6970338126122875519[42] = 0;
   out_6970338126122875519[43] = 0;
   out_6970338126122875519[44] = 0;
   out_6970338126122875519[45] = 0;
   out_6970338126122875519[46] = 0;
   out_6970338126122875519[47] = 0;
   out_6970338126122875519[48] = 1;
   out_6970338126122875519[49] = 0;
   out_6970338126122875519[50] = 0;
   out_6970338126122875519[51] = 0;
   out_6970338126122875519[52] = 0;
   out_6970338126122875519[53] = 0;
   out_6970338126122875519[54] = 0;
   out_6970338126122875519[55] = 0;
   out_6970338126122875519[56] = 0;
   out_6970338126122875519[57] = 0;
   out_6970338126122875519[58] = 0;
   out_6970338126122875519[59] = 0;
   out_6970338126122875519[60] = 1;
   out_6970338126122875519[61] = 0;
   out_6970338126122875519[62] = 0;
   out_6970338126122875519[63] = 0;
   out_6970338126122875519[64] = 0;
   out_6970338126122875519[65] = 0;
   out_6970338126122875519[66] = 0;
   out_6970338126122875519[67] = 0;
   out_6970338126122875519[68] = 0;
   out_6970338126122875519[69] = 0;
   out_6970338126122875519[70] = 0;
   out_6970338126122875519[71] = 0;
   out_6970338126122875519[72] = 1;
   out_6970338126122875519[73] = dt;
   out_6970338126122875519[74] = 0;
   out_6970338126122875519[75] = 0;
   out_6970338126122875519[76] = 0;
   out_6970338126122875519[77] = 0;
   out_6970338126122875519[78] = 0;
   out_6970338126122875519[79] = 0;
   out_6970338126122875519[80] = 0;
   out_6970338126122875519[81] = 0;
   out_6970338126122875519[82] = 0;
   out_6970338126122875519[83] = 0;
   out_6970338126122875519[84] = 1;
   out_6970338126122875519[85] = dt;
   out_6970338126122875519[86] = 0;
   out_6970338126122875519[87] = 0;
   out_6970338126122875519[88] = 0;
   out_6970338126122875519[89] = 0;
   out_6970338126122875519[90] = 0;
   out_6970338126122875519[91] = 0;
   out_6970338126122875519[92] = 0;
   out_6970338126122875519[93] = 0;
   out_6970338126122875519[94] = 0;
   out_6970338126122875519[95] = 0;
   out_6970338126122875519[96] = 1;
   out_6970338126122875519[97] = 0;
   out_6970338126122875519[98] = 0;
   out_6970338126122875519[99] = 0;
   out_6970338126122875519[100] = 0;
   out_6970338126122875519[101] = 0;
   out_6970338126122875519[102] = 0;
   out_6970338126122875519[103] = 0;
   out_6970338126122875519[104] = 0;
   out_6970338126122875519[105] = 0;
   out_6970338126122875519[106] = 0;
   out_6970338126122875519[107] = 0;
   out_6970338126122875519[108] = 1;
   out_6970338126122875519[109] = 0;
   out_6970338126122875519[110] = 0;
   out_6970338126122875519[111] = 0;
   out_6970338126122875519[112] = 0;
   out_6970338126122875519[113] = 0;
   out_6970338126122875519[114] = 0;
   out_6970338126122875519[115] = 0;
   out_6970338126122875519[116] = 0;
   out_6970338126122875519[117] = 0;
   out_6970338126122875519[118] = 0;
   out_6970338126122875519[119] = 0;
   out_6970338126122875519[120] = 1;
}
void h_6(double *state, double *sat_pos, double *out_2137796404954608038) {
   out_2137796404954608038[0] = sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2)) + state[6];
}
void H_6(double *state, double *sat_pos, double *out_6474332688499420280) {
   out_6474332688499420280[0] = (-sat_pos[0] + state[0])/sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2));
   out_6474332688499420280[1] = (-sat_pos[1] + state[1])/sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2));
   out_6474332688499420280[2] = (-sat_pos[2] + state[2])/sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2));
   out_6474332688499420280[3] = 0;
   out_6474332688499420280[4] = 0;
   out_6474332688499420280[5] = 0;
   out_6474332688499420280[6] = 1;
   out_6474332688499420280[7] = 0;
   out_6474332688499420280[8] = 0;
   out_6474332688499420280[9] = 0;
   out_6474332688499420280[10] = 0;
}
void h_20(double *state, double *sat_pos, double *out_3064986068187853552) {
   out_3064986068187853552[0] = sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2)) + sat_pos[3]*state[10] + state[6] + state[9];
}
void H_20(double *state, double *sat_pos, double *out_1015241364259705761) {
   out_1015241364259705761[0] = (-sat_pos[0] + state[0])/sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2));
   out_1015241364259705761[1] = (-sat_pos[1] + state[1])/sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2));
   out_1015241364259705761[2] = (-sat_pos[2] + state[2])/sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2));
   out_1015241364259705761[3] = 0;
   out_1015241364259705761[4] = 0;
   out_1015241364259705761[5] = 0;
   out_1015241364259705761[6] = 1;
   out_1015241364259705761[7] = 0;
   out_1015241364259705761[8] = 0;
   out_1015241364259705761[9] = 1;
   out_1015241364259705761[10] = sat_pos[3];
}
void h_7(double *state, double *sat_pos_vel, double *out_6705706645024539856) {
   out_6705706645024539856[0] = (sat_pos_vel[0] - state[0])*(sat_pos_vel[3] - state[3])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2)) + (sat_pos_vel[1] - state[1])*(sat_pos_vel[4] - state[4])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2)) + (sat_pos_vel[2] - state[2])*(sat_pos_vel[5] - state[5])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2)) + state[7];
}
void H_7(double *state, double *sat_pos_vel, double *out_1489560582702204123) {
   out_1489560582702204123[0] = pow(sat_pos_vel[0] - state[0], 2)*(sat_pos_vel[3] - state[3])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[0] - state[0])*(sat_pos_vel[1] - state[1])*(sat_pos_vel[4] - state[4])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[0] - state[0])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[5] - state[5])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) - (sat_pos_vel[3] - state[3])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_1489560582702204123[1] = (sat_pos_vel[0] - state[0])*(sat_pos_vel[1] - state[1])*(sat_pos_vel[3] - state[3])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + pow(sat_pos_vel[1] - state[1], 2)*(sat_pos_vel[4] - state[4])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[1] - state[1])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[5] - state[5])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) - (sat_pos_vel[4] - state[4])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_1489560582702204123[2] = (sat_pos_vel[0] - state[0])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[3] - state[3])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[1] - state[1])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[4] - state[4])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + pow(sat_pos_vel[2] - state[2], 2)*(sat_pos_vel[5] - state[5])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) - (sat_pos_vel[5] - state[5])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_1489560582702204123[3] = -(sat_pos_vel[0] - state[0])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_1489560582702204123[4] = -(sat_pos_vel[1] - state[1])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_1489560582702204123[5] = -(sat_pos_vel[2] - state[2])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_1489560582702204123[6] = 0;
   out_1489560582702204123[7] = 1;
   out_1489560582702204123[8] = 0;
   out_1489560582702204123[9] = 0;
   out_1489560582702204123[10] = 0;
}
void h_21(double *state, double *sat_pos_vel, double *out_6705706645024539856) {
   out_6705706645024539856[0] = (sat_pos_vel[0] - state[0])*(sat_pos_vel[3] - state[3])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2)) + (sat_pos_vel[1] - state[1])*(sat_pos_vel[4] - state[4])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2)) + (sat_pos_vel[2] - state[2])*(sat_pos_vel[5] - state[5])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2)) + state[7];
}
void H_21(double *state, double *sat_pos_vel, double *out_1489560582702204123) {
   out_1489560582702204123[0] = pow(sat_pos_vel[0] - state[0], 2)*(sat_pos_vel[3] - state[3])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[0] - state[0])*(sat_pos_vel[1] - state[1])*(sat_pos_vel[4] - state[4])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[0] - state[0])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[5] - state[5])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) - (sat_pos_vel[3] - state[3])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_1489560582702204123[1] = (sat_pos_vel[0] - state[0])*(sat_pos_vel[1] - state[1])*(sat_pos_vel[3] - state[3])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + pow(sat_pos_vel[1] - state[1], 2)*(sat_pos_vel[4] - state[4])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[1] - state[1])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[5] - state[5])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) - (sat_pos_vel[4] - state[4])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_1489560582702204123[2] = (sat_pos_vel[0] - state[0])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[3] - state[3])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[1] - state[1])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[4] - state[4])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + pow(sat_pos_vel[2] - state[2], 2)*(sat_pos_vel[5] - state[5])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) - (sat_pos_vel[5] - state[5])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_1489560582702204123[3] = -(sat_pos_vel[0] - state[0])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_1489560582702204123[4] = -(sat_pos_vel[1] - state[1])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_1489560582702204123[5] = -(sat_pos_vel[2] - state[2])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_1489560582702204123[6] = 0;
   out_1489560582702204123[7] = 1;
   out_1489560582702204123[8] = 0;
   out_1489560582702204123[9] = 0;
   out_1489560582702204123[10] = 0;
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
void gnss_err_fun(double *nom_x, double *delta_x, double *out_155765044166347563) {
  err_fun(nom_x, delta_x, out_155765044166347563);
}
void gnss_inv_err_fun(double *nom_x, double *true_x, double *out_5147265498242855152) {
  inv_err_fun(nom_x, true_x, out_5147265498242855152);
}
void gnss_H_mod_fun(double *state, double *out_2441002244825976478) {
  H_mod_fun(state, out_2441002244825976478);
}
void gnss_f_fun(double *state, double dt, double *out_5834456982122438687) {
  f_fun(state,  dt, out_5834456982122438687);
}
void gnss_F_fun(double *state, double dt, double *out_6970338126122875519) {
  F_fun(state,  dt, out_6970338126122875519);
}
void gnss_h_6(double *state, double *sat_pos, double *out_2137796404954608038) {
  h_6(state, sat_pos, out_2137796404954608038);
}
void gnss_H_6(double *state, double *sat_pos, double *out_6474332688499420280) {
  H_6(state, sat_pos, out_6474332688499420280);
}
void gnss_h_20(double *state, double *sat_pos, double *out_3064986068187853552) {
  h_20(state, sat_pos, out_3064986068187853552);
}
void gnss_H_20(double *state, double *sat_pos, double *out_1015241364259705761) {
  H_20(state, sat_pos, out_1015241364259705761);
}
void gnss_h_7(double *state, double *sat_pos_vel, double *out_6705706645024539856) {
  h_7(state, sat_pos_vel, out_6705706645024539856);
}
void gnss_H_7(double *state, double *sat_pos_vel, double *out_1489560582702204123) {
  H_7(state, sat_pos_vel, out_1489560582702204123);
}
void gnss_h_21(double *state, double *sat_pos_vel, double *out_6705706645024539856) {
  h_21(state, sat_pos_vel, out_6705706645024539856);
}
void gnss_H_21(double *state, double *sat_pos_vel, double *out_1489560582702204123) {
  H_21(state, sat_pos_vel, out_1489560582702204123);
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
