//
//                           SimuLTE
// Copyright (C) 2012 Antonio Virdis, Daniele Migliorini, Giovanni
// Accongiagioco, Generoso Pagano, Vincenzo Pii.
//
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself,
// and cannot be removed from it.
//


/*
 *  TODO: scrivere i commenti in inglese!
 */

#ifndef USERTXPARAMS_H_
#define USERTXPARAMS_H_

//#include "LteCommon.h"
#include "LteMcs.h"

/**
 * @class UserTxParams
 *
 * This class specifies the transmission mode and the resources
 * used by a user for the transmission.
 * The class contains also the RI,CQI,PMI actually used for transmission.
 *
 * UserTxParams are set by the AMC Pilot.
 */
class UserTxParams {

    protected:

        TxMode txMode_; // tx mode da usare
        Rank ri_;       // RI
        CqiVector cqiVector_; // vettore con tanti elementi quante sono le cw: quindi contiente dei wb cqi
        Pmi pmi_;       // WB pmi

        std::set<Band> allowedBands_;     // bande su cui l'utente puo' trasmettere

        bool isValid_; // dice se la userinfo è settata o meno

        //! set of Remote Antennas in use for transmission  (DAS support)
        RemoteSet antennaSet_;

    public:

        UserTxParams& operator=(const UserTxParams& other) {
                if (&other==this) return *this;
                this->txMode_=other.txMode_;
                this->ri_=other.ri_;
                this->cqiVector_=other.cqiVector_;
                this->allowedBands_=other.allowedBands_;
                this->isValid_=other.isValid_;
                this->antennaSet_=other.antennaSet_;
                return *this;
            }

        /**
         * Copy constructor: packet copy
         * @param other source packet
         */
        UserTxParams(const UserTxParams& other) {
            operator=(other);
        }
        //! Default constructor. Initialize with default values.
        UserTxParams() {
            restoreDefaultValues();
        }
        //! Reset to default values.
        void restoreDefaultValues() {
            cqiVector_.clear();
            allowedBands_.clear();

            txMode_ = SINGLE_ANTENNA_PORT0;
            ri_ = NORANK;
            pmi_ = NOPMI;
            cqiVector_.push_back(NOSIGNALCQI);

            isValid_ = false;
            antennaSet_.clear();
            // by default the system works with the MACRO antenna configured on all terminals
            antennaSet_.insert(MACRO);
        }
        //! Get/Set the status of the user transmission parameters.
        bool& isSet()
        {
            return isValid_;
        }
        //! Get the transmission mode.
        const TxMode& readTxMode() const {return txMode_;}
        //! Get the RI.
        const Rank& readRank() const {return ri_;}
        //! Get the per-codeword CQIs.
        const std::vector<Cqi>& readCqiVector() const {return cqiVector_;}
        //! Get the PMI.
        const Pmi& readPmi() const {return pmi_;}
        //! Get the assigned band vector.
        const std::set<Band>& readBands() const {return allowedBands_;}
        //! Get the remote antenna set - DAS
        const std::set<Remote>& readAntennaSet() const {return antennaSet_;}

        //! Set the transmission mode.
        void writeTxMode(const TxMode& txMode) {txMode_ = txMode;}
        //! Set the RI.
        void writeRank(const Rank& ri) {ri_ = ri;}
        //! Set the per-codeword CQIs.
        void writeCqi(const std::vector<Cqi>& cqi) {cqiVector_ = cqi;}
        //! Set the PMI.
        void writePmi(const Pmi& pmi) {pmi_ = pmi;}
        //! Set the remote antenna set - DAS
        void writeAntennas(const std::set<Remote>& antennas)  { antennaSet_=antennas;}
        //! Get the assigned band vector.
        void writeBands(const std::set<Band>& bands) {allowedBands_=bands;}

        /** Get the modulation the codeword. This function does not check if codeword is set.
         *  @param cw The codeword.
         *  @return The modulation.
         */
        LteMod getCwModulation(Codeword cw) const {return cqiTable[cqiVector_.at(cw)].mod_;}

        /** Get the nominal code rate given the codeword. This function does not check if codeword is set.
         *  @param cw The codeword.
         *  @return The nominal code rate.
         */
        double getCwRate(Codeword cw) const {return cqiTable[cqiVector_.at(cw)].rate_;}

        /** Gives the number of layers for each codeword.
         *  @return A vector containing the number of layers per codeword.
         */
        std::vector<unsigned char> getLayers() const {
            return cwMapping(txMode_, ri_,ri_);
        }

        /** Print debug information - FOR DEBUG ONLY
         *  @param s The name of the invoking function.
         */
        void print(const char* s) const
        {
            try
            {
                EV << NOW << " " << s << " --------------------------\n";
                EV << NOW << " " << s << "           UserTxParams\n";
                EV << NOW << " " << s << " --------------------------\n";
                EV << NOW << " " << s << " TxMode: "<< txModeToA(txMode_) <<"\n";
                EV << NOW << " " << s << " RI: "<< ri_ <<"\n";

                //*** CQIs *********************************************
                unsigned int codewords = cqiVector_.size();
                EV << NOW << " " << s << " CQI = {";
                if(codewords > 0)
                {
                    EV << cqiVector_.at(0);
                    for(Codeword cw = 1; cw < codewords; ++cw)
                        EV << ", " << cqiVector_.at(cw);
                }
                EV << "}\n";
                //******************************************************

                EV << NOW << " " << s << " PMI: "<< pmi_<<"\n";

                //*** Bands ********************************************
                std::set<Band>::const_iterator it = allowedBands_.begin();
                std::set<Band>::const_iterator et = allowedBands_.end();

                EV << NOW << " " << s << " Bands = {";
                if(it != et)
                {
                    EV << *it;
                    ++it;
                    for(; it != et; ++it)
                        EV << ", "<< *it;
                }
                EV << "}\n";
                //******************************************************


                EV << NOW << " " << s << " --------------------------\n";

            }
            catch(std::exception& e)
            {
                throw cRuntimeError("Exception in UserTxParams::print(): %s", e.what());
            }
        }
};

#endif /* USERTXPARAMS_H_ */