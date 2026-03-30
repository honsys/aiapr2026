#ifndef GEM_ASTRO_HPP
#define GEM_ASTRO_HPP

#include "gem_base.hpp"
#include <cmath>

// Physical and astronomical constants
static constexpr double ASTRO_G       = 6.674e-11;   // gravitational constant (m^3 kg^-1 s^-2)
static constexpr double ASTRO_C       = 2.998e8;     // speed of light (m/s)
static constexpr double ASTRO_AU      = 1.496e11;    // astronomical unit (m)
static constexpr double ASTRO_PC      = 3.086e16;    // parsec (m)
static constexpr double ASTRO_LY      = 9.461e15;    // light-year (m)
static constexpr double ASTRO_MSUN    = 1.989e30;    // solar mass (kg)
static constexpr double ASTRO_RSUN    = 6.957e8;     // solar radius (m)
static constexpr double ASTRO_LSUN    = 3.828e26;    // solar luminosity (W)
static constexpr double ASTRO_TSUN    = 5778.0;      // solar effective temperature (K)
static constexpr double ASTRO_MEARTH  = 5.972e24;    // Earth mass (kg)
static constexpr double ASTRO_REARTH  = 6.371e6;     // Earth radius (m)
static constexpr double ASTRO_H0      = 67.4;        // Hubble constant (km/s/Mpc)
static constexpr double ASTRO_SIGMA   = 5.6704e-8;   // Stefan-Boltzmann (W m^-2 K^-4)
static constexpr double ASTRO_K_B     = 1.3806e-23;  // Boltzmann constant (J/K)
static constexpr double ASTRO_H_PLANCK= 6.626e-34;   // Planck constant (J s)

// Helper: make a GemObject with a name
static std::shared_ptr<GemObject> astro_obj(const std::string& n) {
    return std::make_shared<GemObject>(n);
}

// Helper: safely extract double from a GemValue (handles double or string)
static double astro_d(const std::shared_ptr<GemValue>& v) {
    if (!v) return 0.0;
    if (std::holds_alternative<double>(v->value)) return std::get<double>(v->value);
    if (std::holds_alternative<std::string>(v->value)) {
        try { return std::stod(std::get<std::string>(v->value)); } catch(...) {}
    }
    return 0.0;
}


// ── GemAstro: core astrophysics module ──────────────────────────────────────
class GemAstro : public GemSys {
public:
    GemAstro() : GemSys() {
        name = "astro";

        // ── Constants as properties ──────────────────────────────────────────
        properties["G"]       = std::make_shared<GemValue>(ASTRO_G);
        properties["c"]       = std::make_shared<GemValue>(ASTRO_C);
        properties["AU"]      = std::make_shared<GemValue>(ASTRO_AU);
        properties["pc"]      = std::make_shared<GemValue>(ASTRO_PC);
        properties["ly"]      = std::make_shared<GemValue>(ASTRO_LY);
        properties["Msun"]    = std::make_shared<GemValue>(ASTRO_MSUN);
        properties["Rsun"]    = std::make_shared<GemValue>(ASTRO_RSUN);
        properties["Lsun"]    = std::make_shared<GemValue>(ASTRO_LSUN);
        properties["Tsun"]    = std::make_shared<GemValue>(ASTRO_TSUN);
        properties["Mearth"]  = std::make_shared<GemValue>(ASTRO_MEARTH);
        properties["Rearth"]  = std::make_shared<GemValue>(ASTRO_REARTH);
        properties["H0"]      = std::make_shared<GemValue>(ASTRO_H0);
        properties["sigma_sb"]= std::make_shared<GemValue>(ASTRO_SIGMA);

        // ── Unit conversions ─────────────────────────────────────────────────
        // astro.to_ly(parsecs)  → light-years
        methods["to_ly"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            double pc = std::holds_alternative<double>(a[0]->value) ? astro_d(a[0]) : 0.0;
            return std::make_shared<GemValue>(pc * 3.26156);
        }, true };

        // astro.to_pc(ly)  → parsecs
        methods["to_pc"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            double ly = std::holds_alternative<double>(a[0]->value) ? astro_d(a[0]) : 0.0;
            return std::make_shared<GemValue>(ly / 3.26156);
        }, true };

        // astro.to_au(m)  → AU
        methods["to_au"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            double m = std::holds_alternative<double>(a[0]->value) ? astro_d(a[0]) : 0.0;
            return std::make_shared<GemValue>(m / ASTRO_AU);
        }, true };

        // astro.deg_to_rad(deg), astro.rad_to_deg(rad)
        methods["deg_to_rad"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            return std::make_shared<GemValue>(astro_d(a[0]) * M_PI / 180.0);
        }, true };
        methods["rad_to_deg"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            return std::make_shared<GemValue>(astro_d(a[0]) * 180.0 / M_PI);
        }, true };


        // ── Stellar physics ──────────────────────────────────────────────────
        // astro.luminosity(radius_rsun, temp_K) → L/Lsun
        methods["luminosity"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            if (a.size() < 2) return std::make_shared<GemValue>(0.0);
            double R = astro_d(a[0]);
            double T = astro_d(a[1]);
            return std::make_shared<GemValue>(R*R * std::pow(T/ASTRO_TSUN, 4.0));
        }, true };

        // astro.stefan_boltzmann(temp_K, radius_m) → luminosity W
        methods["stefan_boltzmann"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            if (a.size() < 2) return std::make_shared<GemValue>(0.0);
            double T = astro_d(a[0]);
            double R = astro_d(a[1]);
            return std::make_shared<GemValue>(4.0 * M_PI * R*R * ASTRO_SIGMA * std::pow(T, 4.0));
        }, true };

        // astro.wien(temp_K) → peak wavelength (m)
        methods["wien"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            double T = astro_d(a[0]);
            return std::make_shared<GemValue>(2.898e-3 / T);
        }, true };

        // astro.abs_magnitude(apparent_mag, distance_pc) → absolute magnitude
        methods["abs_magnitude"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            if (a.size() < 2) return std::make_shared<GemValue>(0.0);
            double m = astro_d(a[0]);
            double d = astro_d(a[1]);
            return std::make_shared<GemValue>(m - 5.0 * std::log10(d / 10.0));
        }, true };

        // astro.distance_modulus(dist_pc) → μ
        methods["distance_modulus"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            double d = astro_d(a[0]);
            return std::make_shared<GemValue>(5.0 * std::log10(d) - 5.0);
        }, true };

        // astro.spectral_class(temp_K) → string "O/B/A/F/G/K/M"
        methods["spectral_class"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            double T = astro_d(a[0]);
            std::string cls;
            if      (T >= 30000) cls = "O";
            else if (T >= 10000) cls = "B";
            else if (T >= 7500)  cls = "A";
            else if (T >= 6000)  cls = "F";
            else if (T >= 5200)  cls = "G";
            else if (T >= 3700)  cls = "K";
            else                 cls = "M";
            return std::make_shared<GemValue>(cls);
        }, true };

        // astro.schwarzschild_radius(mass_kg) → r_s (m)
        methods["schwarzschild_radius"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            double M = astro_d(a[0]);
            return std::make_shared<GemValue>(2.0 * ASTRO_G * M / (ASTRO_C * ASTRO_C));
        }, true };

        // astro.escape_velocity(mass_kg, radius_m) → v_esc (m/s)
        methods["escape_velocity"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            if (a.size() < 2) return std::make_shared<GemValue>(0.0);
            double M = astro_d(a[0]);
            double R = astro_d(a[1]);
            return std::make_shared<GemValue>(std::sqrt(2.0 * ASTRO_G * M / R));
        }, true };


        // ── Orbital mechanics ────────────────────────────────────────────────
        // astro.orbital_period(semi_major_au, mass_msun) → period in years (Kepler 3rd)
        methods["orbital_period"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            if (a.size() < 2) return std::make_shared<GemValue>(0.0);
            double a_au = astro_d(a[0]);
            double M    = astro_d(a[1]);
            return std::make_shared<GemValue>(std::sqrt(a_au*a_au*a_au / M));
        }, true };

        // astro.orbital_velocity(semi_major_m, mass_kg) → v (m/s)
        methods["orbital_velocity"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            if (a.size() < 2) return std::make_shared<GemValue>(0.0);
            double a_m = astro_d(a[0]);
            double M   = astro_d(a[1]);
            return std::make_shared<GemValue>(std::sqrt(ASTRO_G * M / a_m));
        }, true };

        // astro.hill_sphere(a_m, m_planet_kg, m_star_kg) → r_Hill (m)
        methods["hill_sphere"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            if (a.size() < 3) return std::make_shared<GemValue>(0.0);
            double a_m = astro_d(a[0]);
            double mp  = astro_d(a[1]);
            double ms  = astro_d(a[2]);
            return std::make_shared<GemValue>(a_m * std::cbrt(mp / (3.0 * ms)));
        }, true };

        // astro.roche_limit(r_primary_m, rho_primary, rho_satellite) → d (m)
        methods["roche_limit"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            if (a.size() < 3) return std::make_shared<GemValue>(0.0);
            double R  = astro_d(a[0]);
            double rp = astro_d(a[1]);
            double rs = astro_d(a[2]);
            return std::make_shared<GemValue>(R * std::cbrt(2.0 * rp / rs));
        }, true };

        // astro.synodic_period(p1_years, p2_years) → synodic period (years)
        methods["synodic_period"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            if (a.size() < 2) return std::make_shared<GemValue>(0.0);
            double p1 = astro_d(a[0]);
            double p2 = astro_d(a[1]);
            return std::make_shared<GemValue>(1.0 / std::abs(1.0/p1 - 1.0/p2));
        }, true };

        // astro.planet(name) → object with known planetary data
        methods["planet"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            if (a.empty()) return std::make_shared<GemValue>();
            std::string n = a[0]->toString();
            // mass(kg), radius(m), semi_major(AU), period(years), moons
            static const std::map<std::string, std::tuple<double,double,double,double,int>> db = {
                {"Mercury", {3.301e23, 2.440e6, 0.387, 0.241, 0}},
                {"Venus",   {4.867e24, 6.052e6, 0.723, 0.615, 0}},
                {"Earth",   {5.972e24, 6.371e6, 1.000, 1.000, 1}},
                {"Mars",    {6.417e23, 3.390e6, 1.524, 1.881, 2}},
                {"Jupiter", {1.898e27, 7.149e7, 5.203, 11.86, 95}},
                {"Saturn",  {5.683e26, 6.027e7, 9.537, 29.46, 146}},
                {"Uranus",  {8.681e25, 2.556e7, 19.19, 84.01, 28}},
                {"Neptune", {1.024e26, 2.476e7, 30.07, 164.8, 16}},
            };
            auto it = db.find(n);
            if (it == db.end()) return std::make_shared<GemValue>();
            auto obj = astro_obj("Planet");
            obj->set("name",       std::make_shared<GemValue>(n));
            obj->set("mass",       std::make_shared<GemValue>(std::get<0>(it->second)));
            obj->set("radius",     std::make_shared<GemValue>(std::get<1>(it->second)));
            obj->set("semi_major", std::make_shared<GemValue>(std::get<2>(it->second)));
            obj->set("period",     std::make_shared<GemValue>(std::get<3>(it->second)));
            obj->set("moons",      std::make_shared<GemValue>((double)std::get<4>(it->second)));
            return std::make_shared<GemValue>(obj);
        }, true };


        // ── Solar physics ────────────────────────────────────────────────────
        // astro.solar_flux(dist_au) → irradiance W/m^2
        methods["solar_flux"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            double d = astro_d(a[0]);
            return std::make_shared<GemValue>(ASTRO_LSUN / (4.0 * M_PI * (d*ASTRO_AU)*(d*ASTRO_AU)));
        }, true };

        // astro.solar_wind_pressure(n_cm3, v_km_s) → dynamic pressure (nPa)
        methods["solar_wind_pressure"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            if (a.size() < 2) return std::make_shared<GemValue>(0.0);
            double n = astro_d(a[0]) * 1e6;   // cm^-3 → m^-3
            double v = astro_d(a[1]) * 1e3;   // km/s → m/s
            double mp = 1.6726e-27;
            return std::make_shared<GemValue>(0.5 * n * mp * v * v * 1e9); // Pa → nPa
        }, true };

        // astro.sunspot_cycle(year) → approximate relative sunspot number (Schwabe ~11yr)
        methods["sunspot_cycle"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            double yr = astro_d(a[0]);
            // Cycle 25 minimum ~2019.8, period 11.0 yr, amplitude ~150
            double phase = 2.0 * M_PI * (yr - 2019.8) / 11.0;
            double ssn = 75.0 * (1.0 - std::cos(phase));
            return std::make_shared<GemValue>(ssn < 0 ? 0.0 : ssn);
        }, true };

        // astro.parker_spiral_angle(v_sw_km_s, dist_au) → angle (deg) of IMF
        methods["parker_spiral_angle"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            if (a.size() < 2) return std::make_shared<GemValue>(0.0);
            double v  = astro_d(a[0]) * 1e3;  // km/s → m/s
            double r  = astro_d(a[1]) * ASTRO_AU;
            double omega = 2.87e-6; // solar rotation rad/s
            double angle = std::atan(omega * r / v) * 180.0 / M_PI;
            return std::make_shared<GemValue>(angle);
        }, true };

        // astro.solar_activity(type) → object with current cycle info
        methods["solar_activity"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            auto obj = astro_obj("SolarActivity");
            obj->set("cycle",       std::make_shared<GemValue>(25.0));
            obj->set("phase",       std::make_shared<GemValue>("Solar Maximum (~2025)"));
            obj->set("ssn_approx",  std::make_shared<GemValue>(150.0));
            obj->set("next_min",    std::make_shared<GemValue>("~2030"));
            return std::make_shared<GemValue>(obj);
        }, true };


        // ── Cosmology ────────────────────────────────────────────────────────
        // astro.hubble_distance(z) → comoving distance (Mpc), flat ΛCDM approx
        methods["hubble_distance"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            double z = astro_d(a[0]);
            // Simple linear approx for small z: d = c*z/H0
            double d = (ASTRO_C / 1e3) * z / ASTRO_H0; // Mpc
            return std::make_shared<GemValue>(d);
        }, true };

        // astro.redshift_velocity(z) → recession velocity (km/s)
        methods["redshift_velocity"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            double z = astro_d(a[0]);
            return std::make_shared<GemValue>(ASTRO_H0 * (ASTRO_C / 1e3) * z / ASTRO_H0);
        }, true };

        // astro.lookback_time(z) → Gyr (flat ΛCDM, Omega_m=0.3, Omega_L=0.7)
        methods["lookback_time"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            double z = astro_d(a[0]);
            // Numerical integration (simple trapezoidal, 1000 steps)
            double H0_s = ASTRO_H0 * 1e3 / (3.086e22); // s^-1
            double Om = 0.3, OL = 0.7;
            int N = 1000;
            double dz = z / N, t = 0.0;
            for (int i = 0; i < N; ++i) {
                double zi = (i + 0.5) * dz;
                double E = std::sqrt(Om * std::pow(1+zi, 3) + OL);
                t += dz / ((1.0 + zi) * E);
            }
            return std::make_shared<GemValue>(t / H0_s / 3.156e16); // Gyr
        }, true };

        // astro.critical_density() → rho_c (kg/m^3)
        methods["critical_density"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            double H0_s = ASTRO_H0 * 1e3 / 3.086e22;
            return std::make_shared<GemValue>(3.0 * H0_s * H0_s / (8.0 * M_PI * ASTRO_G));
        }, true };


        // ── Coordinate transforms ────────────────────────────────────────────
        // astro.equatorial_to_galactic(ra_deg, dec_deg) → {l, b} object
        methods["equatorial_to_galactic"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            if (a.size() < 2) return std::make_shared<GemValue>();
            double ra  = astro_d(a[0]) * M_PI / 180.0;
            double dec = astro_d(a[1]) * M_PI / 180.0;
            // NGP: ra=192.85948°, dec=27.12825°, l_NCP=122.93192°
            double ra_ngp  = 192.85948 * M_PI / 180.0;
            double dec_ngp = 27.12825  * M_PI / 180.0;
            double l_ncp   = 122.93192 * M_PI / 180.0;
            double sin_b = std::sin(dec)*std::sin(dec_ngp) +
                           std::cos(dec)*std::cos(dec_ngp)*std::cos(ra - ra_ngp);
            double b = std::asin(sin_b);
            double cos_b = std::cos(b);
            double sin_l = std::cos(dec)*std::sin(ra - ra_ngp) / cos_b;
            double cos_l = (std::sin(dec) - std::sin(b)*std::sin(dec_ngp)) /
                           (cos_b * std::cos(dec_ngp));
            double l = l_ncp - std::atan2(sin_l, cos_l);
            if (l < 0) l += 2*M_PI;
            auto obj = astro_obj("GalacticCoord");
            obj->set("l", std::make_shared<GemValue>(l * 180.0 / M_PI));
            obj->set("b", std::make_shared<GemValue>(b * 180.0 / M_PI));
            return std::make_shared<GemValue>(obj);
        }, true };

        // astro.angular_separation(ra1,dec1,ra2,dec2) → degrees (Haversine on sphere)
        methods["angular_separation"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            if (a.size() < 4) return std::make_shared<GemValue>(0.0);
            auto d = [](std::shared_ptr<GemValue> v){ return std::get<double>(v->value) * M_PI / 180.0; };
            double ra1=d(a[0]), de1=d(a[1]), ra2=d(a[2]), de2=d(a[3]);
            double dra = ra2-ra1, dde = de2-de1;
            double h = std::sin(dde/2)*std::sin(dde/2) +
                       std::cos(de1)*std::cos(de2)*std::sin(dra/2)*std::sin(dra/2);
            return std::make_shared<GemValue>(2.0 * std::asin(std::sqrt(h)) * 180.0 / M_PI);
        }, true };


        // ── Exoplanet / transit ──────────────────────────────────────────────
        // astro.transit_depth(r_planet_rearth, r_star_rsun) → fractional depth
        methods["transit_depth"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            if (a.size() < 2) return std::make_shared<GemValue>(0.0);
            double rp = astro_d(a[0]) * ASTRO_REARTH;
            double rs = astro_d(a[1]) * ASTRO_RSUN;
            double ratio = rp / rs;
            return std::make_shared<GemValue>(ratio * ratio);
        }, true };

        // astro.habitable_zone(lum_lsun) → {inner_au, outer_au}
        methods["habitable_zone"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            double L = astro_d(a[0]);
            auto obj = astro_obj("HabitableZone");
            obj->set("inner_au", std::make_shared<GemValue>(std::sqrt(L / 1.1)));
            obj->set("outer_au", std::make_shared<GemValue>(std::sqrt(L / 0.53)));
            return std::make_shared<GemValue>(obj);
        }, true };

        // astro.equilibrium_temp(lum_lsun, dist_au, albedo) → T_eq (K)
        methods["equilibrium_temp"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            if (a.size() < 3) return std::make_shared<GemValue>(0.0);
            double L = astro_d(a[0]) * ASTRO_LSUN;
            double d = astro_d(a[1]) * ASTRO_AU;
            double A = astro_d(a[2]);
            double T = std::pow(L*(1.0-A) / (16.0*M_PI*ASTRO_SIGMA*d*d), 0.25);
            return std::make_shared<GemValue>(T);
        }, true };

        // ── Pulsar / neutron star ────────────────────────────────────────────
        // astro.pulsar_spindown(p_s, pdot) → {age_yr, Bfield_G, edot_W}
        methods["pulsar_spindown"] = { [](std::vector<std::shared_ptr<GemValue>> a) {
            if (a.size() < 2) return std::make_shared<GemValue>();
            double P    = astro_d(a[0]);
            double Pdot = astro_d(a[1]);
            double age  = P / (2.0 * Pdot) / 3.156e7; // yr
            double B    = 3.2e19 * std::sqrt(P * Pdot); // Gauss
            double I    = 1e45; // kg m^2 (canonical)
            double Edot = 4.0 * M_PI * M_PI * I * Pdot / (P*P*P);
            auto obj = astro_obj("PulsarSpindown");
            obj->set("age_yr",  std::make_shared<GemValue>(age));
            obj->set("Bfield_G",std::make_shared<GemValue>(B));
            obj->set("edot_W",  std::make_shared<GemValue>(Edot));
            return std::make_shared<GemValue>(obj);
        }, true };

    } // end constructor
}; // end GemAstro

#endif // GEM_ASTRO_HPP
