#include "KSIntDecayCalculatorDeathConstRate.h"
#include "KSInteractionsMessage.h"
#include "KRandom.h"
using katrin::KRandom;

#include "KConst.h"
using katrin::KConst;

namespace Kassiopeia
{

    KSIntDecayCalculatorDeathConstRate::KSIntDecayCalculatorDeathConstRate() :
            fLifeTime( 0. ),
            fTargetPID( 0 ),
            fminPID( 0 ),
            fmaxPID( 0 ),
            fDecayProductGenerator( NULL )
    {
    }

    KSIntDecayCalculatorDeathConstRate::KSIntDecayCalculatorDeathConstRate( const KSIntDecayCalculatorDeathConstRate& aCopy ) :
            KSComponent(),
            fLifeTime( aCopy.fLifeTime ),
            fTargetPID( aCopy.fTargetPID ),
            fminPID( aCopy.fminPID ),
            fmaxPID( aCopy.fmaxPID ),
            fDecayProductGenerator( aCopy.fDecayProductGenerator )
    {
    }

    KSIntDecayCalculatorDeathConstRate* KSIntDecayCalculatorDeathConstRate::Clone() const
    {
        return new KSIntDecayCalculatorDeathConstRate( *this );
    }

    KSIntDecayCalculatorDeathConstRate::~KSIntDecayCalculatorDeathConstRate()
    {
    }

    void KSIntDecayCalculatorDeathConstRate::CalculateLifeTime( const KSParticle& aParticle,
                                                                      double& aLifeTime )
    {
        long long tPID = aParticle.GetPID();
        if ( (tPID == fTargetPID && fTargetPID != 0) || ( (tPID >= fminPID) && ( tPID <= fmaxPID)) )
        {
            aLifeTime = fLifeTime;
        } else
        {
            aLifeTime = std::numeric_limits<double>::max();
        }
        return;
    }

    void KSIntDecayCalculatorDeathConstRate::ExecuteInteraction( const KSParticle& anInitialParticle,
                                                                 KSParticle& aFinalParticle,
                                                                 KSParticleQueue& aSecondaries)
    {
        aFinalParticle.SetTime( anInitialParticle.GetTime() );
        aFinalParticle.SetPosition( anInitialParticle.GetPosition() );
        aFinalParticle.SetMomentum( anInitialParticle.GetMomentum() );

        if ( (anInitialParticle.GetPID() == fTargetPID && fTargetPID != 0) || ( (anInitialParticle.GetPID() >= fminPID) && ( anInitialParticle.GetPID() <= fmaxPID)) )
        {
            double tTime = aFinalParticle.GetTime();
            KThreeVector tPosition = aFinalParticle.GetPosition();

            aFinalParticle.SetLabel( GetName() );                        
            aFinalParticle.SetActive(false);
            if(fDecayProductGenerator != NULL)
            {
                KSParticleQueue tDecayProducts;

                fDecayProductGenerator->ExecuteGeneration(tDecayProducts);

                KSParticleIt tParticleIt;
                for( tParticleIt = tDecayProducts.begin(); tParticleIt != tDecayProducts.end(); tParticleIt++ )
                {
                    (*tParticleIt)->SetTime((*tParticleIt)->GetTime()+tTime);
                    (*tParticleIt)->SetPosition((*tParticleIt)->GetPosition()+tPosition);
                }

                aSecondaries = tDecayProducts;
            }

            fStepNDecays = 1;
            fStepEnergyLoss = 0.;
        }

        return;
    }

    void KSIntDecayCalculatorDeathConstRate::SetDecayProductGenerator( KSGenerator* aGenerator )
    {
        if( fDecayProductGenerator == NULL )
        {
            fDecayProductGenerator = aGenerator;
            return;
        }
        intmsg( eError ) << "cannot set decay product generator <" << aGenerator->GetName() << "> to decay calculator <" << GetName() << ">" << eom;
        return;
    }

    KSGenerator* KSIntDecayCalculatorDeathConstRate::GetDecayProductGenerator() const
    {
        return fDecayProductGenerator;
    }
}

