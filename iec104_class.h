/*
* This software implements an IEC 60870-5-104 protocol tester.
* Copyright ?2010,2011,2012 Ricardo L. Olsen
*
* Disclaimer
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the
* Free Software Foundation, Inc.,
* 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/
#include "stdafx.h"

#ifndef IEC104_CLASS_H
#define IEC104_CLASS_H

// IEC 60870-5-104 BASE CLASS, MASTER IMPLEMENTATION

#include "iec104_types.h"
#include "logmsg.h"

struct iec_obj {
    unsigned int address;  // 3 byte address

    float value; // value

    cp56time2a timetag; // 7 byte time tag
    unsigned char reserved; // for future use

    unsigned char type; // iec type
    unsigned char cause; //
    unsigned short ca;  // common addres of asdu

    union {
        unsigned char ov :1; // overflow/no overflow
        unsigned char sp :1; // single point information
        unsigned char dp :2; // double point information
        unsigned char scs :1; // single command state
        unsigned char dcs :2; // double command state
        unsigned char rcs :2; // regulating step command
    }; // 2 bits

    unsigned char qu :5; // qualifier of command
    unsigned char se :1; // select=1 / execute=0
    // + 6 bits = 8 bits

    unsigned char bl :1; // blocked/not blocked
    unsigned char sb :1; // substituted/not substituted
    unsigned char nt :1; // not topical/topical
    unsigned char iv :1; // valid/invalid
    unsigned char t :1; // transient flag
    unsigned char pn :1; // 0=positive, 1=negative
};

class iec104_class
{
    public:

    static const unsigned int M_SP_NA_1 = 1; // single-point information
    static const unsigned int M_DP_NA_1 = 3; // double-point information
    static const unsigned int M_ST_NA_1 = 5; // step position information
    static const unsigned int M_BO_NA_1 = 7; // bitstring of 32 bits
    static const unsigned int M_ME_NA_1 = 9; // normalized value
    static const unsigned int M_ME_NB_1 = 11; // scaled value
    static const unsigned int M_ME_NC_1 = 13; // floating point
    static const unsigned int M_IT_NA_1 = 15; // �����ۻ���integrated totals
	static const unsigned int M_ME_ND_1 = 21; // ����Ʒ�ʵĹ�һ��ң���
    static const unsigned int M_SP_TB_1 = 30; // single-point information with time tag
    static const unsigned int M_DP_TB_1 = 31; // double-point information with time tag
    static const unsigned int M_ST_TB_1 = 32; // step position information with time tag
    static const unsigned int M_BO_TB_1 = 33; // bitstring of 32 bits with time tag
    static const unsigned int M_ME_TD_1 = 34; // normalized value with time tag
    static const unsigned int M_ME_TE_1 = 35; // scaled value with time tag
    static const unsigned int M_ME_TF_1 = 36; // floating point with time tag
    static const unsigned int M_IT_TB_1 = 37; // integrated totals with time tag
    static const unsigned int C_SC_NA_1 = 45; // single command
    static const unsigned int C_DC_NA_1 = 46; // double command
    static const unsigned int C_RC_NA_1 = 47; // regulating step command
    static const unsigned int C_SC_TA_1 = 58; // single command with time tag
    static const unsigned int C_DC_TA_1 = 59; // double command with time tag
    static const unsigned int C_RC_TA_1 = 60; // regulating step command with time tag
    static const unsigned int M_EI_NA_1 = 70; // end of initialization
    static const unsigned int C_IC_NA_1 = 100; // general interrogation (GI)
    static const unsigned int C_CI_NA_1 = 101; // counter interrogation
    static const unsigned int C_CS_NA_1 = 103; // clock synchronization command
    static const unsigned int C_RP_NA_1 = 105; // reset process command
    static const unsigned int C_TS_TA_1 = 107; // test command with time tag CP56Time2a

    /* cause of transmition (standard) */
    static const unsigned int CYCLIC = 1;
    static const unsigned int BGSCAN = 2;
    static const unsigned int SPONTANEOUS = 3;
    static const unsigned int REQUEST = 5;
    static const unsigned int ACTIVATION = 6;
    static const unsigned int ACTCONFIRM = 7;
    static const unsigned int DEACTIVATION = 8;
    static const unsigned int ACTTERM = 10;

    static const unsigned int SUPERVISORY = 0x01;
    static const unsigned int STARTDTACT = 0x07;
    static const unsigned int STARTDTCON = 0x0B;
    static const unsigned int STOPDTACT = 0x13;
    static const unsigned int STOPDTCON = 0x23;
    static const unsigned int TESTFRACT = 0x43;
    static const unsigned int TESTFRCON = 0x83;
    static const unsigned int INTERROGATION = 0x64;
	static const unsigned int INTEGRATEDTOTALS = 0x65;
    static const unsigned int START = 0x68;
    static const unsigned int RESET = 0x69;

    static const unsigned int POSITIVE = 0;
    static const unsigned int NEGATIVE = 1;

    static const unsigned int SELECT = 1;
    static const unsigned int EXECUTE = 0;

    TLogMsg mLog;

    // ---- user called funcions, must be called by the user -----------------
    iec104_class(); // user called constructor on derived class
    void onConnectTCP(); // user called, when tcp connected
    void onDisconnectTCP(); // user called, when tcp disconnected
    void onTimerSecond();  // user called, each second timer
    void packetReadyTCP(); // user called, when packet ready to be read from tcp connection

    void solicitGI();  // General Interrogation
	void solicitIntegratedTotal();//�ٻ�������
    void setSecondaryIP( char * ip );
    char * getSecondaryIP();
    void setSecondaryAddress( int addr );
    int getSecondaryAddress();
    void setPrimaryAddress( int addr );
    int getPrimaryAddress();
    void disableSequenceOrderCheck();  // allow sequence out of order
    bool sendCommand( iec_obj *obj ); // Command, return false if not send
    int getPortTCP();
    void setPortTCP( unsigned port );

private:
    unsigned short VS;  // sender packet control counter
    unsigned short VR;  // receiver packet control counter
    void confTestCommand(); // test command activation confirmation
    void sendStartDTACT(); // send STARTDTACT
    int tout_startdtact; // timeout control
    void sendSupervisory(); // send supervisory window control frame
    int tout_supervisory;  // countdown to send supervisory window control
    int tout_gi; // countdown to send general interrogation
    int tout_testfr; // countdown to send test frame
    bool connectedTCP; // tcp connection state
    bool seq_order_check; // if set: test message order, disconnect if out of order
    unsigned char masterAddress; // master link address (primary address, originator address, oa)
    unsigned short slaveAddress; // slave link address (secondary address, common address of ASDU, ca)
    unsigned Port; // iec104 tcp port (defaults to 2404)
    char slaveIP[20]; // slave (secondary, RTU) IP address
    static const int t3_testfr = 10;
    static const int t2_supervisory = 8;
    static const int t1_startdtact = 6;

    protected:
    void parseAPDU(iec_apdu * papdu, int sz, bool accountandrespond = true); // parse APDU, ( accountandrespond == false : process the apdu out of the normal handshake )

    int msg_supervisory;

    bool TxOk; // ready to transmit state (STARTDTCON received)
    unsigned GIObjectCnt; // contador de objetos da GI
	unsigned ITObjectCnt; // �����ۻ�������


    // ---- pure virtual funcions, user defined on derived class (mandatory)---

    // make tcp connection, user provided
    virtual void connectTCP() = 0;
    // tcp disconnect, user provided
    virtual void disconnectTCP() = 0;
    // read tcp data, user provided
    virtual int readTCP( char * buf, int szmax ) = 0;
    // send tcp data, user provided
    virtual void sendTCP( char * data, int sz ) = 0;

    // ---- virtual funcions, user defined on derived class (not mandatory)---

    // user point process, user provided. (on one call must be only objects of one type)
    virtual void dataIndication( iec_obj * /*obj*/, int /*numpoints*/){};
    // inform user that ACTCONFIRM of Interrogation was received from slave
    virtual void interrogationActConfIndication(){};
    // inform user that ACTTERM of Interrogation was received from slave
    virtual void interrogationActTermIndication(){};
    // inform user of command activation
    virtual void commandActConfIndication( iec_obj * /*obj*/ ){};
    // inform user of command termination
    virtual void commandActTermIndication( iec_obj * /*obj*/ ){};
    // user process APDU
    virtual void userprocAPDU(iec_apdu * /* papdu */, int /* sz */){};

    // -------------------------------------------------------------------------

};

#endif // IEC104_CLASS_H


