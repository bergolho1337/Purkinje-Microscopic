/* ********************************************************* */
// NOBLE
/* ********************************************************* */
// Dividir tudo por 1000 para mudar o modelo de (s ---> ms)

// Quatro EDOs para resolver
const int num_equacoes = 4;

// Nome do modelo celular
const char model_name[10] = "Noble";

const double v_stim = -87.005;		// mV
//double v_stim = -87.0;		// mV
const double v_gate = -87.0;			// mV

// Initial conditions
const double V0 = -8.0e+01;				// mV
const double m0 = 1.0e-02;
const double h0 = 8.0e-01;
const double n0 = 1.0e-02;

// Parameters:
const double Cm = 1.0e-08;				// uF/um^2 (Bruno: 1.0uF/cm^2 = 1.0e-08 uF/um^2)
const double g_Na_max = 4.0e+05;
const double E_Na = 4.0e+01;
const double g_L = 7.5e+01;
const double E_L = -6.0e+01;
const double CM = 1.2e+01;


// Define o tipo Func, ponteiro para função que recebe t, um vetor y e o intervalo de tempo k como parâmetro e retorna a avaliação da função
typedef
	double (*Func) (int k, double t, double *y);

/* ********************************************************************************************************************** */
// dV/dt

inline double calc_i_Leak (double V_old)
{
	return ((g_L*(V_old-E_L)));
}

inline double calc_g_K2 (double n_old)
{
	return ((1.2e+03*pow(n_old,4.0e+00)));
}

inline double calc_g_K1 (double V_old)
{
	return (((1.2e+03*exp((((-V_old)-9.0e+01)/5.0e+01)))+(1.5e+01*exp(((V_old+9.0e+01)/6.0e+01)))));
}

inline double calc_i_K (double V_old, double n_old)
{
	return (((calc_g_K1(V_old)+calc_g_K2(n_old))*(V_old+1.0e+02)));
}

inline double calc_g_Na (double m_old, double h_old)
{
	return ((pow(m_old,3.0e+00)*h_old*g_Na_max));
}

// Changing the 1.4e+02 to 1.225e+02 we can eliminate the auto-oscilatory behaviour of the Noble model
inline double calc_i_Na (double V_old, double m_old, double h_old)
{
	return ((calc_g_Na(m_old,h_old)+1.4e+02)*(V_old-E_Na));
}

inline double dvdt (int k, double t, double *y)
{
	if (y[0] < v_gate)
		return (0);
	else
		return ((-(calc_i_Na(y[0],y[1],y[2])+calc_i_K(y[0],y[3])+calc_i_Leak(y[0])))/CM);
		//return ((-(calc_i_Na(y[0],y[1],y[2])+calc_i_K(y[0],y[3])+calc_i_Leak(y[0]))));
}

/* ********************************************************************************************************************** */
// dm/dt

inline double calc_beta_m (double V_old)
{
	return (((1.2e+02*(V_old+8.0e+00))/(exp(((V_old+8.0e+00)/5.0e+00))-1.0e+00)));
}

inline double calc_alpha_m (double V_old)
{
	return (((1.0e+02*((-V_old)-4.8e+01))/(exp((((-V_old)-4.8e+01)/1.5e+01))-1.0e+00)));
}

inline double dmdt (int k, double t, double *y)
{
	if (y[0] < v_gate)
		return (0);
	else
		return ((calc_alpha_m(y[0])*(1.0e+00-y[1]))-(calc_beta_m(y[0])*y[1]));
}

/* ********************************************************************************************************************** */
// dh/dt

inline double calc_beta_h (double V_old)
{
	return ((1.0e+03/(1.0e+00+exp((((-V_old)-4.2e+01)/1.0e+01)))));
}

inline double calc_alpha_h (double V_old)
{
	return ((1.7e+02*exp((((-V_old)-9.0e+01)/2.0e+01))));
}

inline double dhdt (int k, double t, double *y)
{
	if (y[0] < v_gate)
		return (0);
	else
		return ((calc_alpha_h(y[0])*(1.0e+00-y[2]))-(calc_beta_h(y[0])*y[2]));
}

/* ********************************************************************************************************************** */
// dn/dt

inline double calc_beta_n (double V_old)
{
	return ((2.0e+00*exp((((-V_old)-9.0e+01)/8.0e+01))));
}

inline double calc_alpha_n (double V_old)
{
	return (((1.0e-01*((-V_old)-5.0e+01))/(exp((((-V_old)-5.0e+01)/1.0e+01))-1.0e+00)));
}

inline double dndt (int k, double t, double *y)
{
	if (y[0] < v_gate)
		return (0);
	else
		return ((calc_alpha_n(y[0])*(1.0e+00-y[3]))-(calc_beta_n(y[0])*y[3]));
}


/* ********************************************************************************************************************** */
// [!] Essas funções todos modelos devem implementar !!!!
inline double* getInitialConditions ()
{
	double *y_old = new double[num_equacoes+1];
	y_old[0] = V0;
	y_old[1] = m0;
	y_old[2] = h0;
	y_old[3] = n0;
	y_old[4] = v_stim;
	return (y_old);
}

inline Func* getFunctions ()
{
	Func *f = new Func[num_equacoes];
	f[0] = dvdt;
	f[1] = dmdt;
	f[2] = dhdt;
	f[3] = dndt;
	return (f);
}
