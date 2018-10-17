#ifndef GQCG_RDMS_HPP
#define GQCG_RDMS_HPP


#include "RDM/OneRDM.hpp"
#include "RDM/TwoRDM.hpp"


namespace GQCG {


struct OneRDMs {

    OneRDM one_rdm;  // spin-summed (total) 1-RDM

    OneRDM one_rdm_aa;  // alpha-alpha (a-a) 1-RDM
    OneRDM one_rdm_bb;  // beta-beta (b-b) 1-RDM

    // CONSTRUCTORS
    /**
     *  Constructor with @param one_rdm
     *  where one_rdm_aa and one_rdm_bb are @param one_rdm/2
     */
    OneRDMs(const OneRDM& one_rdm);

    /**
     *  Constructor with @param one_rdm_aa and @param one_rdm_bb
     *  were one_rdm = @param one_rdm_aa + @param one_rdm_bb
     */
    OneRDMs(const OneRDM& one_rdm_aa, const OneRDM& one_rdm_bb);

};


struct TwoRDMs {

    TwoRDM two_rdm;  // spin-summed (total) 2-RDM

    TwoRDM two_rdm_aaaa;  // a-a-a-a 2-RDM
    TwoRDM two_rdm_aabb;  // a-a-b-b 2-RDM
    TwoRDM two_rdm_bbaa;  // b-a-a-b 2-RDM
    TwoRDM two_rdm_bbbb;  // b-b-b-b 2-RDM


    // CONSTRUCTORS
    /**
     *  Constructor
     *  where two_rdm = @param two_rdm_aaaa + @param two_rdm_aabb + @param two_rdm_bbaa  + @param two_rdm_bbbb
     */
    TwoRDMs(const TwoRDM& two_rdm_aaaa, const TwoRDM& two_rdm_aabb, const TwoRDM& two_rdm_bbaa, const TwoRDM& two_rdm_bbbb);

};


}  // namespace GQCG

#endif  // GQCG_RDMS_HPP