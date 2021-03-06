/*
 * KElectricZHFieldSolver.cc
 *
 *  Created on: 23.07.2015
 *      Author: gosda
 */

#include "KElectricZHFieldSolver.hh"
#include <string>
#include <vector>
#include "KEMCout.hh"
#include "KMD5HashGenerator.hh"
#include "KEMFileInterface.hh"

#include "KElectrostaticBoundaryIntegratorFactory.hh"

#include "KElectrostaticZonalHarmonicFieldSolver.hh"
#include "KZonalHarmonicContainer.hh"
#include "KZonalHarmonicParameters.hh"

//#include "KElectrostaticBasis.hh"

#ifdef KEMFIELD_USE_MPI
#include "KMPIInterface.hh"
#endif

#ifdef KEMFIELD_USE_MPI
    #ifndef MPI_SINGLE_PROCESS
        #define MPI_SINGLE_PROCESS if ( KEMField::KMPIInterface::GetInstance()->GetProcess()==0 )
    #endif
#else
    #ifndef MPI_SINGLE_PROCESS
        #define MPI_SINGLE_PROCESS if( true )
    #endif
#endif

using std::string;
using std::vector;

namespace KEMField {

KElectricZHFieldSolver::KElectricZHFieldSolver() :
							fIntegrator(KEBIFactory::MakeDefault()),
                    		fZHContainer( NULL ),
							fZonalHarmonicFieldSolver( NULL )
{
	fParameters = new KZonalHarmonicParameters();
}

KElectricZHFieldSolver::~KElectricZHFieldSolver()
{
    delete fZHContainer;
    delete fZonalHarmonicFieldSolver;
    delete fParameters;
}

void KElectricZHFieldSolver::InitializeCore( KSurfaceContainer& container )
    {
        // compute hash of the solved geometry
        KMD5HashGenerator solutionHashGenerator;
        string solutionHash = solutionHashGenerator.GenerateHash( container );

        //KEMField::cout << "<shape+boundary+solution> hash is <" << solutionHash << ">" << KEMField::endl;

        // compute hash of the parameter values on the bare geometry
        KMD5HashGenerator parameterHashGenerator;
        string parameterHash = parameterHashGenerator.GenerateHash( *fParameters );

        //KEMField::cout << "<parameter> hash is <" << parameterHash << ">" << KEMField::endl;

        // create label set for zh container object
        string zhContainerBase( KZonalHarmonicContainer< KElectrostaticBasis >::Name() );
        string zhContainerName = zhContainerBase + string( "_" ) + solutionHash + string( "_" ) + parameterHash;
        vector< string > zhContainerLabels;
        zhContainerLabels.push_back( zhContainerBase );
        zhContainerLabels.push_back( solutionHash );
        zhContainerLabels.push_back( parameterHash );

        fZHContainer = new KZonalHarmonicContainer< KElectrostaticBasis >( container, fParameters );

        bool containerFound = false;

        KEMFileInterface::GetInstance()->FindByLabels( *fZHContainer, zhContainerLabels, 0, containerFound );

        if( containerFound == true )
        {
            KEMField::cout << "zonal harmonic container found." << KEMField::endl;
        }
        else
        {
            //KEMField::cout << "no zonal harmonic container found." << KEMField::endl;

            fZHContainer->ComputeCoefficients();

            MPI_SINGLE_PROCESS
            {
                KEMFileInterface::GetInstance()->Write( *fZHContainer, zhContainerName, zhContainerLabels );
            }
        }

        fIntegrator = fIntegratorPolicy.CreateIntegrator();
        fZonalHarmonicFieldSolver = new KZonalHarmonicFieldSolver< KElectrostaticBasis >( *fZHContainer, fIntegrator );
        fZonalHarmonicFieldSolver->Initialize();

        return;
    }

    double KElectricZHFieldSolver::PotentialCore( const KPosition& P ) const
    {
        return fZonalHarmonicFieldSolver->Potential( P );
    }

    KEMThreeVector KElectricZHFieldSolver::ElectricFieldCore( const KPosition& P ) const
    {
        return fZonalHarmonicFieldSolver->ElectricField( P );
    }

    std::pair<KEMThreeVector,double>
    KElectricZHFieldSolver::ElectricFieldAndPotentialCore(const KPosition& P) const
    {
        return fZonalHarmonicFieldSolver->ElectricFieldAndPotential(P);
    }

    bool KElectricZHFieldSolver::UseCentralExpansion(const KPosition &P)
    {
        return fZonalHarmonicFieldSolver->UseCentralExpansion( P );
    }

    bool KElectricZHFieldSolver::UseRemoteExpansion(const KPosition &P)
    {
        return fZonalHarmonicFieldSolver->UseRemoteExpansion( P );
    }


} /* namespace KEMField */
