#ifndef C_MOM_DISTRIB_Fe56_Ben_H
#define C_MOM_DISTRIB_Fe56_Ben_H

#include "GConstants.h"
#include "CLorentzDistrib.h"
#include "dirs.h"


//O. Benhar
//Momentum distribution normalized to 1 at interval [0:800 MeV/c]
// notation: k = momentum in fm^-1
//			 p = momentum in MeV 
class CMomDistrib_Fe56_Ben: public CMomDistrib
{
public:
	CMomDistrib_Fe56_Ben()
		:m_pRes(42),
		 m_pStep(20.0),
		 m_coeff( 1.0/(4.0*pi/fermi3)  ),
		 m_lorCenterV(174.0),
		 m_lorHalfWidth(82.0),
		 m_lorCoeff(0.0415)
	{
		m_Distrib = new double[m_pRes];
		m_lorentz = new CLorentzDistrib(m_lorCenterV, m_lorHalfWidth);

		std::ifstream inputFile;
		open_data_file(inputFile, "sf/Fe56_Ben.dat" );
		
		if (inputFile.fail())
		{
			std::cerr<<"Indispensable file 'Fe56_Ben.dat' not found"<<std::endl;
			return;
		}

		double p(0), val(0.0);

		for (int iCnt(0); iCnt<m_pRes; ++iCnt)
		{
			inputFile>>p>>val;
			m_Distrib[iCnt] = val*(4*pi);
		}

	};

	double Tot(const double p) const;
	double MF(const double p) const;
	double Corr(const double p) const;
	double  TotMean() const { return 182.117;}

	double generate() const;

private:
	const int m_pRes;
	const double m_pStep;
	const double m_coeff;

	mutable double* m_Distrib;
	
	const double m_lorCenterV;
	const double m_lorHalfWidth;
	const double m_lorCoeff;

	const CLorentzDistrib *m_lorentz;
	
};


double CMomDistrib_Fe56_Ben::Tot(const double p) const
{
	//p = m_pStep*(np-0.5) + pR
	if ( p >= 790.0 )
		return m_coeff*0.0375*exp(-0.284*p*p*fermi2);

	const int np( static_cast<int>((p+0.5*m_pStep)/m_pStep) ); 
	
	const double pR( p-m_pStep*(np-0.5) );
	
	const double c0( m_Distrib[np]   );
	const double c1( m_Distrib[np+1] );

	double f( (c1 - c0)*pR/m_pStep + c0 );

	return m_coeff*f;
}

double CMomDistrib_Fe56_Ben::MF(const double p) const
{
	//av momentum 182.117 MeV/c
	const double k2(p*p*fermi2);

	if ( p <= 330.0 )
		return Tot(p) - m_coeff*( 0.230*exp(-1.2*k2) + 0.038405*exp(-0.282*k2) );

	return  0.0;
	
}

double CMomDistrib_Fe56_Ben::Corr(const double p) const
{

	//Corr part is 20.225% of normalization for protons and neutrons
	const double k2(p*p*fermi2);

	if ( p <= 330.0 )
		return m_coeff*( 0.230*exp(-1.2*k2) + 0.038405*exp(-0.282*k2) );
			
	return  Tot(p); 

}



double CMomDistrib_Fe56_Ben::generate() const
{
	while (true)
	{
		const double p( m_lorentz->generate() );
		
		if ( p<0.0 || p>800.0 )
			continue;

		if ( frandom()*m_lorCoeff*m_lorentz->eval(p) < p*p*Tot(p) )
			return p;
	}

	return 0.0;
}

#endif
