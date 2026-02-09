struct transfer_data
{
  u_int32_t exp_id;
  u_int32_t run_id;
  u_int32_t key_id;
  u_int32_t counter;
  u_int32_t size1;
  timeval depttime;
  char junk[1500];
};
