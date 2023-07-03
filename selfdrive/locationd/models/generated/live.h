#pragma once
#include "rednose/helpers/common_ekf.h"
extern "C" {
void live_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_9(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_12(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_35(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_32(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_33(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_H(double *in_vec, double *out_1097356199825971220);
void live_err_fun(double *nom_x, double *delta_x, double *out_568252403372226262);
void live_inv_err_fun(double *nom_x, double *true_x, double *out_7213038466475256783);
void live_H_mod_fun(double *state, double *out_5343623426000789985);
void live_f_fun(double *state, double dt, double *out_4932771627039192882);
void live_F_fun(double *state, double dt, double *out_2767945159801779539);
void live_h_4(double *state, double *unused, double *out_1451480361000935061);
void live_H_4(double *state, double *unused, double *out_4577228633181462700);
void live_h_9(double *state, double *unused, double *out_7158285452497808853);
void live_H_9(double *state, double *unused, double *out_4818418279811053345);
void live_h_10(double *state, double *unused, double *out_5588464198051389910);
void live_H_10(double *state, double *unused, double *out_2165770699822960725);
void live_h_12(double *state, double *unused, double *out_8210021402501847002);
void live_H_12(double *state, double *unused, double *out_5198327658229056367);
void live_h_35(double *state, double *unused, double *out_809595733831829358);
void live_H_35(double *state, double *unused, double *out_7943890690554070076);
void live_h_32(double *state, double *unused, double *out_4707117236553835774);
void live_H_32(double *state, double *unused, double *out_1029026186859953720);
void live_h_13(double *state, double *unused, double *out_8923808646549831194);
void live_H_13(double *state, double *unused, double *out_6163092163170966406);
void live_h_14(double *state, double *unused, double *out_7158285452497808853);
void live_H_14(double *state, double *unused, double *out_4818418279811053345);
void live_h_33(double *state, double *unused, double *out_7418478399482253382);
void live_H_33(double *state, double *unused, double *out_7352296378516623936);
void live_predict(double *in_x, double *in_P, double *in_Q, double dt);
}