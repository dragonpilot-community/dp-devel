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
void live_H(double *in_vec, double *out_5465641885577763319);
void live_err_fun(double *nom_x, double *delta_x, double *out_2162730634078729501);
void live_inv_err_fun(double *nom_x, double *true_x, double *out_2784842220323608381);
void live_H_mod_fun(double *state, double *out_8629437051351605834);
void live_f_fun(double *state, double dt, double *out_9119723707155781877);
void live_F_fun(double *state, double dt, double *out_2926342148693202044);
void live_h_4(double *state, double *unused, double *out_122350829928085436);
void live_H_4(double *state, double *unused, double *out_3125739590203371811);
void live_h_9(double *state, double *unused, double *out_3706110223310072950);
void live_H_9(double *state, double *unused, double *out_4161479345061075659);
void live_h_10(double *state, double *unused, double *out_1500255286745539056);
void live_H_10(double *state, double *unused, double *out_58979021465123944);
void live_h_12(double *state, double *unused, double *out_6311704777123009525);
void live_H_12(double *state, double *unused, double *out_8939746106463446809);
void live_h_35(double *state, double *unused, double *out_3352441690342662506);
void live_H_35(double *state, double *unused, double *out_6761434934921091098);
void live_h_32(double *state, double *unused, double *out_4786345732318478291);
void live_H_32(double *state, double *unused, double *out_8798827715940006022);
void live_h_13(double *state, double *unused, double *out_1755976025828354517);
void live_H_13(double *state, double *unused, double *out_4270406951243891899);
void live_h_14(double *state, double *unused, double *out_3706110223310072950);
void live_H_14(double *state, double *unused, double *out_4161479345061075659);
void live_h_33(double *state, double *unused, double *out_414582735699757719);
void live_H_33(double *state, double *unused, double *out_3610877930282233494);
void live_predict(double *in_x, double *in_P, double *in_Q, double dt);
}