/*
 * KVTKViewerVisitorBuilder.hh
 *
 *  Created on: 30 Jul 2015
 *      Author: wolfgang
 */

#ifndef KEMFIELD_SOURCE_2_0_PLUGINS_BINDINGS_FIELDS_ELECTRIC_INCLUDE_KVTKVIEWERVISITORBUILDER_HH_
#define KEMFIELD_SOURCE_2_0_PLUGINS_BINDINGS_FIELDS_ELECTRIC_INCLUDE_KVTKVIEWERVISITORBUILDER_HH_

#include "KVTKViewerAsBoundaryFieldVisitor.hh"
#include "KComplexElement.hh"


namespace katrin {

typedef KComplexElement<KEMField::KVTKViewerAsBoundaryFieldVisitor>
KVTKViewerVisitorBuilder;

template< >
inline bool KVTKViewerVisitorBuilder::AddAttribute( KContainer* aContainer )
{
    if( aContainer->GetName() == "file" )
    {
        std::string name = "";
        aContainer->CopyTo( name );
        fObject->SetFile( name );


        return true;
    }
    if( aContainer->GetName() == "view" )
    {
        bool choice = false;
        aContainer->CopyTo( choice );
        fObject->ViewGeometry( choice );
        return true;
    }
    if( aContainer->GetName() == "save" )
    {
        bool choice = false;
        aContainer->CopyTo( choice );
        fObject->SaveGeometry( choice );
        return true;
    }
    if( aContainer->GetName() == "preprocessing" )
    {
        bool choice = false;
        aContainer->CopyTo( choice );
        fObject->Preprocessing( choice );
        return true;
    }
    if( aContainer->GetName() == "postprocessing" )
    {
        bool choice = false;
        aContainer->CopyTo( choice );
        fObject->Postprocessing( choice );
        return true;
    }
    return false;
}


} /* namespace katrin */

#endif /* KEMFIELD_SOURCE_2_0_PLUGINS_BINDINGS_FIELDS_ELECTRIC_INCLUDE_KVTKVIEWERVISITORBUILDER_HH_ */
