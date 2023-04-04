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
void live_H(double *in_vec, double *out_6296950856560866654);
void live_err_fun(double *nom_x, double *delta_x, double *out_2899646891747653485);
void live_inv_err_fun(double *nom_x, double *true_x, double *out_8154559542040542297);
void live_H_mod_fun(double *state, double *out_4390300350310679035);
void live_f_fun(double *state, double dt, double *out_8268551332720580037);
void live_F_fun(double *state, double dt, double *out_1164008229608083872);
void live_h_4(double *state, double *unused, double *out_2262361584068828184);
void live_H_4(double *state, double *unused, double *out_8086601223300992224);
void live_h_9(double *state, double *unused, double *out_4303923515652599437);
void live_H_9(double *state, double *unused, double *out_7845411576671401579);
void live_h_10(double *state, double *unused, double *out_3990839466442438769);
void live_H_10(double *state, double *unused, double *out_6460755846635292532);
void live_h_12(double *state, double *unused, double *out_8824834610017378240);
void live_H_12(double *state, double *unused, double *out_3067144815269030429);
void live_h_35(double *state, double *unused, double *out_3004299637425634318);
void live_H_35(double *state, double *unused, double *out_4719939165928384848);
void live_h_32(double *state, double *unused, double *out_8458500521587119713);
void live_H_32(double *state, double *unused, double *out_6364778675639284450);
void live_h_13(double *state, double *unused, double *out_241671530149260146);
void live_H_13(double *state, double *unused, double *out_2100740473575830672);
void live_h_14(double *state, double *unused, double *out_4303923515652599437);
void live_H_14(double *state, double *unused, double *out_7845411576671401579);
void live_h_33(double *state, double *unused, double *out_8838711856581645412);
void live_H_33(double *state, double *unused, double *out_1569382161289527244);
void live_predict(double *in_x, double *in_P, double *in_Q, double dt);
}