/*! 
  \file
  Create atmospheric data file with initial air parcel positions.
*/

#include "libtrac.h"

int main(
  int argc,
  char *argv[]) {

  atm_t *atm;

  ctl_t ctl;

  gsl_rng *rng;

  double dt, dz, dlon, dlat, lat0, lat1, lon0, lon1, t0, t1, z0, z1,
    t, z, lon, lat, ut, uz, ulon, ulat, m;

  double theta = 0, phi = 0;
  
  int ip, irep, rep;

  unsigned long seed;

  /* Allocate... */
  ALLOC(atm, atm_t, 1);

  /* Check arguments... */
  if (argc < 3)
    ERRMSG("Give parameters: <ctl> <atm_out>");

  /* Read control parameters... */
  read_ctl(NULL, NULL, argc, argv, &ctl);
  t0 = scan_ctl(NULL, NULL, argc, argv, "INIT_T0", -1, "0", NULL);
  t1 = scan_ctl(NULL, NULL, argc, argv, "INIT_T1", -1, "0", NULL);
  dt = scan_ctl(NULL, NULL, argc, argv, "INIT_DT", -1, "1", NULL);
  z0 = scan_ctl(NULL, NULL, argc, argv, "INIT_Z0", -1, "0", NULL);
  z1 = scan_ctl(NULL, NULL, argc, argv, "INIT_Z1", -1, "0", NULL);
  dz = scan_ctl(NULL, NULL, argc, argv, "INIT_DZ", -1, "1", NULL);
  lon0 = scan_ctl(NULL, NULL, argc, argv, "INIT_LON0", -1, "0", NULL);
  lon1 = scan_ctl(NULL, NULL, argc, argv, "INIT_LON1", -1, "0", NULL);
  dlon = scan_ctl(NULL, NULL, argc, argv, "INIT_DLON", -1, "1", NULL);
  lat0 = scan_ctl(NULL, NULL, argc, argv, "INIT_LAT0", -1, "0", NULL);
  lat1 = scan_ctl(NULL, NULL, argc, argv, "INIT_LAT1", -1, "0", NULL);
  dlat = scan_ctl(NULL, NULL, argc, argv, "INIT_DLAT", -1, "1", NULL);
  ut = scan_ctl(NULL, NULL, argc, argv, "INIT_UT", -1, "0", NULL);
  uz = scan_ctl(NULL, NULL, argc, argv, "INIT_UZ", -1, "0", NULL);
  ulon = scan_ctl(NULL, NULL, argc, argv, "INIT_ULON", -1, "0", NULL);
  ulat = scan_ctl(NULL, NULL, argc, argv, "INIT_ULAT", -1, "0", NULL);
  rep = (int) scan_ctl(NULL, NULL, argc, argv, "INIT_REP", -1, "1", NULL);
  m = scan_ctl(NULL, NULL, argc, argv, "INIT_MASS", -1, "0", NULL);
  seed = (unsigned long) scan_ctl(NULL, NULL, argc, argv, "INIT_SEED", -1, "0", NULL);

  /* Initialize random number generator... */
  gsl_rng_env_setup();
  rng = gsl_rng_alloc(gsl_rng_default);
  if (seed != 0) {
    gsl_rng_set(rng, (unsigned long)seed);
  }
  /* Create grid... */
  for (t = t0; t <= t1; t += dt)
    for (z = z0; z <= z1; z += dz)
      for (lon = lon0; lon <= lon1; lon += dlon)
	for (lat = lat0; lat <= lat1; lat += dlat)
	  for (irep = 0; irep < rep; irep++) {

	    /* Set position... */
	    atm->time[atm->np]
	      = (t  + ut * (gsl_rng_uniform(rng) - 0.5));
	    atm->p[atm->np]
	      = P(z + uz * (gsl_rng_uniform(rng) - 0.5));
          
        theta = 2 * M_PI * gsl_rng_uniform(rng);
        phi = acos(2*gsl_rng_uniform(rng)-1.0);
        
	    atm->lon[atm->np] = theta*180.0/(M_PI)-180;
	    atm->lat[atm->np] = phi*180.0/(M_PI)-90;

        while(atm->lat[atm->np] < lat-ulat/2 || atm->lat[atm->np] > lat+ulat/2) {
            theta = 2 * M_PI * gsl_rng_uniform(rng);
            phi = acos(2*gsl_rng_uniform(rng)-1.0);        
            atm->lon[atm->np] = theta*180.0/(M_PI)-180;
            atm->lat[atm->np] = phi*180.0/(M_PI)-90;
        }
        
	    /* Set particle counter... */
	    if ((++atm->np) >= NP)
	      ERRMSG("Too many particles!");
	  }

  /* Check number of air parcels... */
  if (atm->np <= 0)
    ERRMSG("Did not create any air parcels!");

  /* Initialize mass... */
  if (ctl.qnt_mass >= 0)
    for (ip = 0; ip < atm->np; ip++)
      atm->q[ctl.qnt_mass][ip] = m / atm->np;

  /* Save data... */
  write_atm(NULL, argv[2], atm, &ctl);

  /* Free... */
  gsl_rng_free(rng);
  free(atm);

  return EXIT_SUCCESS;
}
