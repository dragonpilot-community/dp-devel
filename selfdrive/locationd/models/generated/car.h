#pragma once
#include "rednose/helpers/common_ekf.h"
extern "C" {
void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_err_fun(double *nom_x, double *delta_x, double *out_4218563854107170996);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_1544528291709365585);
void car_H_mod_fun(double *state, double *out_3035036435099682035);
void car_f_fun(double *state, double dt, double *out_1150570653314300042);
void car_F_fun(double *state, double dt, double *out_5640151155408725370);
void car_h_25(double *state, double *unused, double *out_4669821610217864848);
void car_H_25(double *state, double *unused, double *out_6258512777106223790);
void car_h_24(double *state, double *unused, double *out_5751839562146525567);
void car_H_24(double *state, double *unused, double *out_4745556703614611506);
void car_h_30(double *state, double *unused, double *out_4945015672502370737);
void car_H_30(double *state, double *unused, double *out_6387851724249463860);
void car_h_26(double *state, double *unused, double *out_933269815056344351);
void car_H_26(double *state, double *unused, double *out_8446727977729271602);
void car_h_27(double *state, double *unused, double *out_4756379051051210326);
void car_H_27(double *state, double *unused, double *out_8562615036049888771);
void car_h_29(double *state, double *unused, double *out_4913565719402339471);
void car_H_29(double *state, double *unused, double *out_8170766310790111812);
void car_h_28(double *state, double *unused, double *out_7291323505244492144);
void car_H_28(double *state, double *unused, double *out_3088367293720581238);
void car_h_31(double *state, double *unused, double *out_4827008278568993993);
void car_H_31(double *state, double *unused, double *out_6227866815229263362);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}