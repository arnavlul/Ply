#include <iostream>
#include <string>
#include <cstdint>
#include <bit>
#include <vector>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <atomic>
#include <fstream>

using namespace std;

class Board;

namespace Polyglot {
    const uint64_t RANDOM_ARRAY[781] = {
        0x9D39247E33776D41, 0x2AF7398005AAA5C7, 0x44DB015024623547, 0x9C15F73E62A76AE2,
        0x75834465489C0C89, 0x3290AC3A203001BF, 0x0FBBAD1F61042279, 0xE83A908FF2FB60CA,
        0x0D7E765D58755C10, 0x1A083822CEAFE02D, 0x9605D5F0E25EC3B0, 0xD021FF5CD13A2ED5,
        0x40BDF15D4A672E32, 0x011355146FD56395, 0x5DB4832046F3D9E5, 0x239F8B2D7FF719CC,
        0x05D1A1AE85B49AA1, 0x679F848F6E8FC971, 0x7449BBFF801FED0B, 0x7D11CDB1C3B7ADF0,
        0x82C7709E781EB7CC, 0xF3218F1C9510786C, 0x331478F3AF51BBE6, 0x4BB38DE5E7219443,
        0xAA649C6EBCFD50FC, 0x8DBD98A352AFD40B, 0x87D2074B81D79217, 0x19F3C751D3E92AE1,
        0xB4AB30F062B19ABF, 0x7B0500AC42047AC4, 0xC9452CA81A09D85D, 0x24AA6C514DA27500,
        0x4C9F34427501B447, 0x14A68FD73C910841, 0xA71B9B83461CBD93, 0x03488B95B0F1850F,
        0x637B2B34FF93C040, 0x09D1BC9A3DD90A94, 0x3575668334A1DD3B, 0x735E2B97A4C45A23,
        0x18727070F1BD400B, 0x1FCBACD259BF02E7, 0xD310A7C2CE9B6555, 0xBF983FE0FE5D8244,
        0x9F74D14F7454A824, 0x51EBDC4AB9BA3035, 0x5C82C505DB9AB0FA, 0xFCF7FE8A3430B241,
        0x3253A729B9BA3DDE, 0x8C74C368081B3075, 0xB9BC6C87167C33E7, 0x7EF48F2B83024E20,
        0x11D505D4C351BD7F, 0x6568FCA92C76A243, 0x4DE0B0F40F32A7B8, 0x96D693460CC37E5D,
        0x42E240CB63689F2F, 0x6D2BDCDAE2919661, 0x42880B0236E4D951, 0x5F0F4A5898171BB6,
        0x39F890F579F92F88, 0x93C5B5F47356388B, 0x63DC359D8D231B78, 0xEC16CA8AEA98AD76,
        0x5355F900C2A82DC7, 0x07FB9F855A997142, 0x5093417AA8A7ED5E, 0x7BCBC38DA25A7F3C,
        0x19FC8A768CF4B6D4, 0x637A7780DECFC0D9, 0x8249A47AEE0E41F7, 0x79AD695501E7D1E8,
        0x14ACBAF4777D5776, 0xF145B6BECCDEA195, 0xDABF2AC8201752FC, 0x24C3C94DF9C8D3F6,
        0xBB6E2924F03912EA, 0x0CE26C0B95C980D9, 0xA49CD132BFBF7CC4, 0xE99D662AF4243939,
        0x27E6AD7891165C3F, 0x8535F040B9744FF1, 0x54B3F4FA5F40D873, 0x72B12C32127FED2B,
        0xEE954D3C7B411F47, 0x9A85AC909A24EAA1, 0x70AC4CD9F04F21F5, 0xF9B89D3E99A075C2,
        0x87B3E2B2B5C907B1, 0xA366E5B8C54F48B8, 0xAE4A9346CC3F7CF2, 0x1920C04D47267BBD,
        0x87BF02C6B49E2AE9, 0x092237AC237F3859, 0xFF07F64EF8ED14D0, 0x8DE8DCA9F03CC54E,
        0x9C1633264DB49C89, 0xB3F22C3D0B0B38ED, 0x390E5FB44D01144B, 0x5BFEA5B4712768E9,
        0x1E1032911FA78984, 0x9A74ACB964E78CB3, 0x4F80F7A035DAFB04, 0x6304D09A0B3738C4,
        0x2171E64683023A08, 0x5B9B63EB9CEFF80C, 0x506AACF489889342, 0x1881AFC9A3A701D6,
        0x6503080440750644, 0xDFD395339CDBF4A7, 0xEF927DBCF00C20F2, 0x7B32F7D1E03680EC,
        0xB9FD7620E7316243, 0x05A7E8A57DB91B77, 0xB5889C6E15630A75, 0x4A750A09CE9573F7,
        0xCF464CEC899A2F8A, 0xF538639CE705B824, 0x3C79A0FF5580EF7F, 0xEDE6C87F8477609D,
        0x799E81F05BC93F31, 0x86536B8CF3428A8C, 0x97D7374C60087B73, 0xA246637CFF328532,
        0x043FCAE60CC0EBA0, 0x920E449535DD359E, 0x70EB093B15B290CC, 0x73A1921916591CBD,
        0x56436C9FE1A1AA8D, 0xEFAC4B70633B8F81, 0xBB215798D45DF7AF, 0x45F20042F24F1768,
        0x930F80F4E8EB7462, 0xFF6712FFCFD75EA1, 0xAE623FD67468AA70, 0xDD2C5BC84BC8D8FC,
        0x7EED120D54CF2DD9, 0x22FE545401165F1C, 0xC91800E98FB99929, 0x808BD68E6AC10365,
        0xDEC468145B7605F6, 0x1BEDE3A3AEF53302, 0x43539603D6C55602, 0xAA969B5C691CCB7A,
        0xA87832D392EFEE56, 0x65942C7B3C7E11AE, 0xDED2D633CAD004F6, 0x21F08570F420E565,
        0xB415938D7DA94E3C, 0x91B859E59ECB6350, 0x10CFF333E0ED804A, 0x28AED140BE0BB7DD,
        0xC5CC1D89724FA456, 0x5648F680F11A2741, 0x2D255069F0B7DAB3, 0x9BC5A38EF729ABD4,
        0xEF2F054308F6A2BC, 0xAF2042F5CC5C2858, 0x480412BAB7F5BE2A, 0xAEF3AF4A563DFE43,
        0x19AFE59AE451497F, 0x52593803DFF1E840, 0xF4F076E65F2CE6F0, 0x11379625747D5AF3,
        0xBCE5D2248682C115, 0x9DA4243DE836994F, 0x066F70B33FE09017, 0x4DC4DE189B671A1C,
        0x51039AB7712457C3, 0xC07A3F80C31FB4B4, 0xB46EE9C5E64A6E7C, 0xB3819A42ABE61C87,
        0x21A007933A522A20, 0x2DF16F761598AA4F, 0x763C4A1371B368FD, 0xF793C46702E086A0,
        0xD7288E012AEB8D31, 0xDE336A2A4BC1C44B, 0x0BF692B38D079F23, 0x2C604A7A177326B3,
        0x4850E73E03EB6064, 0xCFC447F1E53C8E1B, 0xB05CA3F564268D99, 0x9AE182C8BC9474E8,
        0xA4FC4BD4FC5558CA, 0xE755178D58FC4E76, 0x69B97DB1A4C03DFE, 0xF9B5B7C4ACC67C96,
        0xFC6A82D64B8655FB, 0x9C684CB6C4D24417, 0x8EC97D2917456ED0, 0x6703DF9D2924E97E,
        0xC547F57E42A7444E, 0x78E37644E7CAD29E, 0xFE9A44E9362F05FA, 0x08BD35CC38336615,
        0x9315E5EB3A129ACE, 0x94061B871E04DF75, 0xDF1D9F9D784BA010, 0x3BBA57B68871B59D,
        0xD2B7ADEEDED1F73F, 0xF7A255D83BC373F8, 0xD7F4F2448C0CEB81, 0xD95BE88CD210FFA7,
        0x336F52F8FF4728E7, 0xA74049DAC312AC71, 0xA2F61BB6E437FDB5, 0x4F2A5CB07F6A35B3,
        0x87D380BDA5BF7859, 0x16B9F7E06C453A21, 0x7BA2484C8A0FD54E, 0xF3A678CAD9A2E38C,
        0x39B0BF7DDE437BA2, 0xFCAF55C1BF8A4424, 0x18FCF680573FA594, 0x4C0563B89F495AC3,
        0x40E087931A00930D, 0x8CFFA9412EB642C1, 0x68CA39053261169F, 0x7A1EE967D27579E2,
        0x9D1D60E5076F5B6F, 0x3810E399B6F65BA2, 0x32095B6D4AB5F9B1, 0x35CAB62109DD038A,
        0xA90B24499FCFAFB1, 0x77A225A07CC2C6BD, 0x513E5E634C70E331, 0x4361C0CA3F692F12,
        0xD941ACA44B20A45B, 0x528F7C8602C5807B, 0x52AB92BEB9613989, 0x9D1DFA2EFC557F73,
        0x722FF175F572C348, 0x1D1260A51107FE97, 0x7A249A57EC0C9BA2, 0x04208FE9E8F7F2D6,
        0x5A110C6058B920A0, 0x0CD9A497658A5698, 0x56FD23C8F9715A4C, 0x284C847B9D887AAE,
        0x04FEABFBBDB619CB, 0x742E1E651C60BA83, 0x9A9632E65904AD3C, 0x881B82A13B51B9E2,
        0x506E6744CD974924, 0xB0183DB56FFC6A79, 0x0ED9B915C66ED37E, 0x5E11E86D5873D484,
        0xF678647E3519AC6E, 0x1B85D488D0F20CC5, 0xDAB9FE6525D89021, 0x0D151D86ADB73615,
        0xA865A54EDCC0F019, 0x93C42566AEF98FFB, 0x99E7AFEABE000731, 0x48CBFF086DDF285A,
        0x7F9B6AF1EBF78BAF, 0x58627E1A149BBA21, 0x2CD16E2ABD791E33, 0xD363EFF5F0977996,
        0x0CE2A38C344A6EED, 0x1A804AADB9CFA741, 0x907F30421D78C5DE, 0x501F65EDB3034D07,
        0x37624AE5A48FA6E9, 0x957BAF61700CFF4E, 0x3A6C27934E31188A, 0xD49503536ABCA345,
        0x088E049589C432E0, 0xF943AEE7FEBF21B8, 0x6C3B8E3E336139D3, 0x364F6FFA464EE52E,
        0xD60F6DCEDC314222, 0x56963B0DCA418FC0, 0x16F50EDF91E513AF, 0xEF1955914B609F93,
        0x565601C0364E3228, 0xECB53939887E8175, 0xBAC7A9A18531294B, 0xB344C470397BBA52,
        0x65D34954DAF3CEBD, 0xB4B81B3FA97511E2, 0xB422061193D6F6A7, 0x071582401C38434D,
        0x7A13F18BBEDC4FF5, 0xBC4097B116C524D2, 0x59B97885E2F2EA28, 0x99170A5DC3115544,
        0x6F423357E7C6A9F9, 0x325928EE6E6F8794, 0xD0E4366228B03343, 0x565C31F7DE89EA27,
        0x30F5611484119414, 0xD873DB391292ED4F, 0x7BD94E1D8E17DEBC, 0xC7D9F16864A76E94,
        0x947AE053EE56E63C, 0xC8C93882F9475F5F, 0x3A9BF55BA91F81CA, 0xD9A11FBB3D9808E4,
        0x0FD22063EDC29FCA, 0xB3F256D8ACA0B0B9, 0xB03031A8B4516E84, 0x35DD37D5871448AF,
        0xE9F6082B05542E4E, 0xEBFAFA33D7254B59, 0x9255ABB50D532280, 0xB9AB4CE57F2D34F3,
        0x693501D628297551, 0xC62C58F97DD949BF, 0xCD454F8F19C5126A, 0xBBE83F4ECC2BDECB,
        0xDC842B7E2819E230, 0xBA89142E007503B8, 0xA3BC941D0A5061CB, 0xE9F6760E32CD8021,
        0x09C7E552BC76492F, 0x852F54934DA55CC9, 0x8107FCCF064FCF56, 0x098954D51FFF6580,
        0x23B70EDB1955C4BF, 0xC330DE426430F69D, 0x4715ED43E8A45C0A, 0xA8D7E4DAB780A08D,
        0x0572B974F03CE0BB, 0xB57D2E985E1419C7, 0xE8D9ECBE2CF3D73F, 0x2FE4B17170E59750,
        0x11317BA87905E790, 0x7FBF21EC8A1F45EC, 0x1725CABFCB045B00, 0x964E915CD5E2B207,
        0x3E2B8BCBF016D66D, 0xBE7444E39328A0AC, 0xF85B2B4FBCDE44B7, 0x49353FEA39BA63B1,
        0x1DD01AAFCD53486A, 0x1FCA8A92FD719F85, 0xFC7C95D827357AFA, 0x18A6A990C8B35EBD,
        0xCCCB7005C6B9C28D, 0x3BDBB92C43B17F26, 0xAA70B5B4F89695A2, 0xE94C39A54A98307F,
        0xB7A0B174CFF6F36E, 0xD4DBA84729AF48AD, 0x2E18BC1AD9704A68, 0x2DE0966DAF2F8B1C,
        0xB9C11D5B1E43A07E, 0x64972D68DEE33360, 0x94628D38D0C20584, 0xDBC0D2B6AB90A559,
        0xD2733C4335C6A72F, 0x7E75D99D94A70F4D, 0x6CED1983376FA72B, 0x97FCAACBF030BC24,
        0x7B77497B32503B12, 0x8547EDDFB81CCB94, 0x79999CDFF70902CB, 0xCFFE1939438E9B24,
        0x829626E3892D95D7, 0x92FAE24291F2B3F1, 0x63E22C147B9C3403, 0xC678B6D860284A1C,
        0x5873888850659AE7, 0x0981DCD296A8736D, 0x9F65789A6509A440, 0x9FF38FED72E9052F,
        0xE479EE5B9930578C, 0xE7F28ECD2D49EECD, 0x56C074A581EA17FE, 0x5544F7D774B14AEF,
        0x7B3F0195FC6F290F, 0x12153635B2C0CF57, 0x7F5126DBBA5E0CA7, 0x7A76956C3EAFB413,
        0x3D5774A11D31AB39, 0x8A1B083821F40CB4, 0x7B4A38E32537DF62, 0x950113646D1D6E03,
        0x4DA8979A0041E8A9, 0x3BC36E078F7515D7, 0x5D0A12F27AD310D1, 0x7F9D1A2E1EBE1327,
        0xDA3A361B1C5157B1, 0xDCDD7D20903D0C25, 0x36833336D068F707, 0xCE68341F79893389,
        0xAB9090168DD05F34, 0x43954B3252DC25E5, 0xB438C2B67F98E5E9, 0x10DCD78E3851A492,
        0xDBC27AB5447822BF, 0x9B3CDB65F82CA382, 0xB67B7896167B4C84, 0xBFCED1B0048EAC50,
        0xA9119B60369FFEBD, 0x1FFF7AC80904BF45, 0xAC12FB171817EEE7, 0xAF08DA9177DDA93D,
        0x1B0CAB936E65C744, 0xB559EB1D04E5E932, 0xC37B45B3F8D6F2BA, 0xC3A9DC228CAAC9E9,
        0xF3B8B6675A6507FF, 0x9FC477DE4ED681DA, 0x67378D8ECCEF96CB, 0x6DD856D94D259236,
        0xA319CE15B0B4DB31, 0x073973751F12DD5E, 0x8A8E849EB32781A5, 0xE1925C71285279F5,
        0x74C04BF1790C0EFE, 0x4DDA48153C94938A, 0x9D266D6A1CC0542C, 0x7440FB816508C4FE,
        0x13328503DF48229F, 0xD6BF7BAEE43CAC40, 0x4838D65F6EF6748F, 0x1E152328F3318DEA,
        0x8F8419A348F296BF, 0x72C8834A5957B511, 0xD7A023A73260B45C, 0x94EBC8ABCFB56DAE,
        0x9FC10D0F989993E0, 0xDE68A2355B93CAE6, 0xA44CFE79AE538BBE, 0x9D1D84FCCE371425,
        0x51D2B1AB2DDFB636, 0x2FD7E4B9E72CD38C, 0x65CA5B96B7552210, 0xDD69A0D8AB3B546D,
        0x604D51B25FBF70E2, 0x73AA8A564FB7AC9E, 0x1A8C1E992B941148, 0xAAC40A2703D9BEA0,
        0x764DBEAE7FA4F3A6, 0x1E99B96E70A9BE8B, 0x2C5E9DEB57EF4743, 0x3A938FEE32D29981,
        0x26E6DB8FFDF5ADFE, 0x469356C504EC9F9D, 0xC8763C5B08D1908C, 0x3F6C6AF859D80055,
        0x7F7CC39420A3A545, 0x9BFB227EBDF4C5CE, 0x89039D79D6FC5C5C, 0x8FE88B57305E2AB6,
        0xA09E8C8C35AB96DE, 0xFA7E393983325753, 0xD6B6D0ECC617C699, 0xDFEA21EA9E7557E3,
        0xB67C1FA481680AF8, 0xCA1E3785A9E724E5, 0x1CFC8BED0D681639, 0xD18D8549D140CAEA,
        0x4ED0FE7E9DC91335, 0xE4DBF0634473F5D2, 0x1761F93A44D5AEFE, 0x53898E4C3910DA55,
        0x734DE8181F6EC39A, 0x2680B122BAA28D97, 0x298AF231C85BAFAB, 0x7983EED3740847D5,
        0x66C1A2A1A60CD889, 0x9E17E49642A3E4C1, 0xEDB454E7BADC0805, 0x50B704CAB602C329,
        0x4CC317FB9CDDD023, 0x66B4835D9EAFEA22, 0x219B97E26FFC81BD, 0x261E4E4C0A333A9D,
        0x1FE2CCA76517DB90, 0xD7504DFA8816EDBB, 0xB9571FA04DC089C8, 0x1DDC0325259B27DE,
        0xCF3F4688801EB9AA, 0xF4F5D05C10CAB243, 0x38B6525C21A42B0E, 0x36F60E2BA4FA6800,
        0xEB3593803173E0CE, 0x9C4CD6257C5A3603, 0xAF0C317D32ADAA8A, 0x258E5A80C7204C4B,
        0x8B889D624D44885D, 0xF4D14597E660F855, 0xD4347F66EC8941C3, 0xE699ED85B0DFB40D,
        0x2472F6207C2D0484, 0xC2A1E7B5B459AEB5, 0xAB4F6451CC1D45EC, 0x63767572AE3D6174,
        0xA59E0BD101731A28, 0x116D0016CB948F09, 0x2CF9C8CA052F6E9F, 0x0B090A7560A968E3,
        0xABEEDDB2DDE06FF1, 0x58EFC10B06A2068D, 0xC6E57A78FBD986E0, 0x2EAB8CA63CE802D7,
        0x14A195640116F336, 0x7C0828DD624EC390, 0xD74BBE77E6116AC7, 0x804456AF10F5FB53,
        0xEBE9EA2ADF4321C7, 0x03219A39EE587A30, 0x49787FEF17AF9924, 0xA1E9300CD8520548,
        0x5B45E522E4B1B4EF, 0xB49C3B3995091A36, 0xD4490AD526F14431, 0x12A8F216AF9418C2,
        0x001F837CC7350524, 0x1877B51E57A764D5, 0xA2853B80F17F58EE, 0x993E1DE72D36D310,
        0xB3598080CE64A656, 0x252F59CF0D9F04BB, 0xD23C8E176D113600, 0x1BDA0492E7E4586E,
        0x21E0BD5026C619BF, 0x3B097ADAF088F94E, 0x8D14DEDB30BE846E, 0xF95CFFA23AF5F6F4,
        0x3871700761B3F743, 0xCA672B91E9E4FA16, 0x64C8E531BFF53B55, 0x241260ED4AD1E87D,
        0x106C09B972D2E822, 0x7FBA195410E5CA30, 0x7884D9BC6CB569D8, 0x0647DFEDCD894A29,
        0x63573FF03E224774, 0x4FC8E9560F91B123, 0x1DB956E450275779, 0xB8D91274B9E9D4FB,
        0xA2EBEE47E2FBFCE1, 0xD9F1F30CCD97FB09, 0xEFED53D75FD64E6B, 0x2E6D02C36017F67F,
        0xA9AA4D20DB084E9B, 0xB64BE8D8B25396C1, 0x70CB6AF7C2D5BCF0, 0x98F076A4F7A2322E,
        0xBF84470805E69B5F, 0x94C3251F06F90CF3, 0x3E003E616A6591E9, 0xB925A6CD0421AFF3,
        0x61BDD1307C66E300, 0xBF8D5108E27E0D48, 0x240AB57A8B888B20, 0xFC87614BAF287E07,
        0xEF02CDD06FFDB432, 0xA1082C0466DF6C0A, 0x8215E577001332C8, 0xD39BB9C3A48DB6CF,
        0x2738259634305C14, 0x61CF4F94C97DF93D, 0x1B6BACA2AE4E125B, 0x758F450C88572E0B,
        0x959F587D507A8359, 0xB063E962E045F54D, 0x60E8ED72C0DFF5D1, 0x7B64978555326F9F,
        0xFD080D236DA814BA, 0x8C90FD9B083F4558, 0x106F72FE81E2C590, 0x7976033A39F7D952,
        0xA4EC0132764CA04B, 0x733EA705FAE4FA77, 0xB4D8F77BC3E56167, 0x9E21F4F903B33FD9,
        0x9D765E419FB69F6D, 0xD30C088BA61EA5EF, 0x5D94337FBFAF7F5B, 0x1A4E4822EB4D7A59,
        0x6FFE73E81B637FB3, 0xDDF957BC36D8B9CA, 0x64D0E29EEA8838B3, 0x08DD9BDFD96B9F63,
        0x087E79E5A57D1D13, 0xE328E230E3E2B3FB, 0x1C2559E30F0946BE, 0x720BF5F26F4D2EAA,
        0xB0774D261CC609DB, 0x443F64EC5A371195, 0x4112CF68649A260E, 0xD813F2FAB7F5C5CA,
        0x660D3257380841EE, 0x59AC2C7873F910A3, 0xE846963877671A17, 0x93B633ABFA3469F8,
        0xC0C0F5A60EF4CDCF, 0xCAF21ECD4377B28C, 0x57277707199B8175, 0x506C11B9D90E8B1D,
        0xD83CC2687A19255F, 0x4A29C6465A314CD1, 0xED2DF21216235097, 0xB5635C95FF7296E2,
        0x22AF003AB672E811, 0x52E762596BF68235, 0x9AEBA33AC6ECC6B0, 0x944F6DE09134DFB6,
        0x6C47BEC883A7DE39, 0x6AD047C430A12104, 0xA5B1CFDBA0AB4067, 0x7C45D833AFF07862,
        0x5092EF950A16DA0B, 0x9338E69C052B8E7B, 0x455A4B4CFE30E3F5, 0x6B02E63195AD0CF8,
        0x6B17B224BAD6BF27, 0xD1E0CCD25BB9C169, 0xDE0C89A556B9AE70, 0x50065E535A213CF6,
        0x9C1169FA2777B874, 0x78EDEFD694AF1EED, 0x6DC93D9526A50E68, 0xEE97F453F06791ED,
        0x32AB0EDB696703D3, 0x3A6853C7E70757A7, 0x31865CED6120F37D, 0x67FEF95D92607890,
        0x1F2B1D1F15F6DC9C, 0xB69E38A8965C6B65, 0xAA9119FF184CCCF4, 0xF43C732873F24C13,
        0xFB4A3D794A9A80D2, 0x3550C2321FD6109C, 0x371F77E76BB8417E, 0x6BFA9AAE5EC05779,
        0xCD04F3FF001A4778, 0xE3273522064480CA, 0x9F91508BFFCFC14A, 0x049A7F41061A9E60,
        0xFCB6BE43A9F2FE9B, 0x08DE8A1C7797DA9B, 0x8F9887E6078735A1, 0xB5B4071DBFC73A66,
        0x230E343DFBA08D33, 0x43ED7F5A0FAE657D, 0x3A88A0FBBCB05C63, 0x21874B8B4D2DBC4F,
        0x1BDEA12E35F6A8C9, 0x53C065C6C8E63528, 0xE34A1D250E7A8D6B, 0xD6B04D3B7651DD7E,
        0x5E90277E7CB39E2D, 0x2C046F22062DC67D, 0xB10BB459132D0A26, 0x3FA9DDFB67E2F199,
        0x0E09B88E1914F7AF, 0x10E8B35AF3EEAB37, 0x9EEDECA8E272B933, 0xD4C718BC4AE8AE5F,
        0x81536D601170FC20, 0x91B534F885818A06, 0xEC8177F83F900978, 0x190E714FADA5156E,
        0xB592BF39B0364963, 0x89C350C893AE7DC1, 0xAC042E70F8B383F2, 0xB49B52E587A1EE60,
        0xFB152FE3FF26DA89, 0x3E666E6F69AE2C15, 0x3B544EBE544C19F9, 0xE805A1E290CF2456,
        0x24B33C9D7ED25117, 0xE74733427B72F0C1, 0x0A804D18B7097475, 0x57E3306D881EDB4F,
        0x4AE7D6A36EB5DBCB, 0x2D8D5432157064C8, 0xD1E649DE1E7F268B, 0x8A328A1CEDFE552C,
        0x07A3AEC79624C7DA, 0x84547DDC3E203C94, 0x990A98FD5071D263, 0x1A4FF12616EEFC89,
        0xF6F7FD1431714200, 0x30C05B1BA332F41C, 0x8D2636B81555A786, 0x46C9FEB55D120902,
        0xCCEC0A73B49C9921, 0x4E9D2827355FC492, 0x19EBB029435DCB0F, 0x4659D2B743848A2C,
        0x963EF2C96B33BE31, 0x74F85198B05A2E7D, 0x5A0F544DD2B1FB18, 0x03727073C2E134B1,
        0xC7F6AA2DE59AEA61, 0x352787BAA0D7C22F, 0x9853EAB63B5E0B35, 0xABBDCDD7ED5C0860,
        0xCF05DAF5AC8D77B0, 0x49CAD48CEBF4A71E, 0x7A4C10EC2158C4A6, 0xD9E92AA246BF719E,
        0x13AE978D09FE5557, 0x730499AF921549FF, 0x4E4B705B92903BA4, 0xFF577222C14F0A3A,
        0x55B6344CF97AAFAE, 0xB862225B055B6960, 0xCAC09AFBDDD2CDB4, 0xDAF8E9829FE96B5F,
        0xB5FDFC5D3132C498, 0x310CB380DB6F7503, 0xE87FBB46217A360E, 0x2102AE466EBB1148,
        0xF8549E1A3AA5E00D, 0x07A69AFDCC42261A, 0xC4C118BFE78FEAAE, 0xF9F4892ED96BD438,
        0x1AF3DBE25D8F45DA, 0xF5B4B0B0D2DEEEB4, 0x962ACEEFA82E1C84, 0x046E3ECAAF453CE9,
        0xF05D129681949A4C, 0x964781CE734B3C84, 0x9C2ED44081CE5FBD, 0x522E23F3925E319E,
        0x177E00F9FC32F791, 0x2BC60A63A6F3B3F2, 0x222BBFAE61725606, 0x486289DDCC3D6780,
        0x7DC7785B8EFDFC80, 0x8AF38731C02BA980, 0x1FAB64EA29A2DDF7, 0xE4D9429322CD065A,
        0x9DA058C67844F20C, 0x24C0E332B70019B0, 0x233003B5A6CFE6AD, 0xD586BD01C5C217F6,
        0x5E5637885F29BC2B, 0x7EBA726D8C94094B, 0x0A56A5F0BFE39272, 0xD79476A84EE20D06,
        0x9E4C1269BAA4BF37, 0x17EFEE45B0DEE640, 0x1D95B0A5FCF90BC6, 0x93CBE0B699C2585D,
        0x65FA4F227A2B6D79, 0xD5F9E858292504D5, 0xC2B5A03F71471A6F, 0x59300222B4561E00,
        0xCE2F8642CA0712DC, 0x7CA9723FBB2E8988, 0x2785338347F2BA08, 0xC61BB3A141E50E8C,
        0x150F361DAB9DEC26, 0x9F6A419D382595F4, 0x64A53DC924FE7AC9, 0x142DE49FFF7A7C3D,
        0x0C335248857FA9E7, 0x0A9C32D5EAE45305, 0xE6C42178C4BBB92E, 0x71F1CE2490D20B07,
        0xF1BCC3D275AFE51A, 0xE728E8C83C334074, 0x96FBF83A12884624, 0x81A1549FD6573DA5,
        0x5FA7867CAF35E149, 0x56986E2EF3ED091B, 0x917F1DD5F8886C61, 0xD20D8C88C8FFE65F,
        0x31D71DCE64B2C310, 0xF165B587DF898190, 0xA57E6339DD2CF3A0, 0x1EF6E6DBB1961EC9,
        0x70CC73D90BC26E24, 0xE21A6B35DF0C3AD7, 0x003A93D8B2806962, 0x1C99DED33CB890A1,
        0xCF3145DE0ADD4289, 0xD0E4427A5514FB72, 0x77C621CC9FB3A483, 0x67A34DAC4356550B,
        0xF8D626AAAF278509
    };

    inline uint64_t swap64(uint64_t v) {
        return ((v & 0xFFULL) << 56) | ((v & 0xFF00ULL) << 40) | ((v & 0xFF0000ULL) << 24) | ((v & 0xFF000000ULL) << 8) |
               ((v >> 8) & 0xFF000000ULL) | ((v >> 24) & 0xFF0000ULL) | ((v >> 40) & 0xFF00ULL) | ((v >> 56) & 0xFFULL);
    }
    inline uint16_t swap16(uint16_t v) { return (v << 8) | (v >> 8); }

#pragma pack(push, 1)
    struct Entry {
        uint64_t key;
        uint16_t move;
        uint16_t weight;
        uint32_t learn;
    };
#pragma pack(pop)

    uint16_t getBookMove(uint64_t key, const Board& board);
}

namespace Evaluation {
    const int PAWN_VAL = 100;
    const int KNIGHT_VAL = 320;
    const int BISHOP_VAL = 330;
    const int ROOK_VAL = 500;
    const int QUEEN_VAL = 900;
    const int KING_VAL = 20000;

    const int pawn_pst[64] = {
         0,  0,  0,  0,  0,  0,  0,  0,
         5, 10, 10,-20,-20, 10, 10,  5,
         5, -5,-10,  0,  0,-10, -5,  5,
         0,  0,  0, 20, 20,  0,  0,  0,
         5,  5, 10, 25, 25, 10,  5,  5,
        10, 10, 20, 30, 30, 20, 10, 10,
        50, 50, 50, 50, 50, 50, 50, 50,
         0,  0,  0,  0,  0,  0,  0,  0
    };

    const int knight_pst[64] = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50
    };

    const int bishop_pst[64] = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
    };

    const int rook_pst[64] = {
          0,  0,  0,  5,  5,  0,  0,  0,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
         -5,  0,  0,  0,  0,  0,  0, -5,
          5, 10, 10, 10, 10, 10, 10,  5,
          0,  0,  0,  0,  0,  0,  0,  0
    };

    const int queen_pst[64] = {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -10,  5,  5,  5,  5,  5,  0,-10,
          0,  0,  5,  5,  5,  5,  0, -5,
         -5,  0,  5,  5,  5,  5,  0, -5,
        -10,  0,  5,  5,  5,  5,  0,-10,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    };

    const int king_pst[64] = {
         20, 30, 10,  0,  0, 10, 30, 20,
         20, 20,  0,  0,  0,  0, 20, 20,
        -10,-20,-20,-20,-20,-20,-20,-10,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30
    };

    uint64_t pieceKeys[12][64];
    uint64_t castleKeys[16];
    uint64_t enPassantKeys[8];
    uint64_t sideKey;

    uint64_t random64() {
        static uint64_t seed = 1070372ULL;
        seed ^= seed >> 12;
        seed ^= seed << 25;
        seed ^= seed >> 27;
        return seed * 0x2545F4914F6CDD1DULL;
    }

    void initZobrist() {
        for (int i = 0; i < 12; i++)
            for (int j = 0; j < 64; j++)
                pieceKeys[i][j] = random64();
        for (int i = 0; i < 16; i++) castleKeys[i] = random64();
        for (int i = 0; i < 8; i++) enPassantKeys[i] = random64();
        sideKey = random64();
    }
}

class Board {
private:
    uint64_t whitePawn;
    uint64_t whiteKing;
    uint64_t whiteQueen;
    uint64_t whiteBishop;
    uint64_t whiteKnight;
    uint64_t whiteRook;

    uint64_t blackPawn;
    uint64_t blackKing;
    uint64_t blackQueen;
    uint64_t blackBishop;
    uint64_t blackKnight;
    uint64_t blackRook;

    uint64_t knightMoveMask[64];
    uint64_t kingMoveMask[64];

    uint8_t castlingRights;
    const uint8_t WK = 1, WQ = 2, BK = 4, BQ = 8;
    uint8_t castlingMask[64];
    int enPassantSquare;
    bool sideToMove;
    int halfmoveClock;
    int fullmoveNumber;
    uint64_t hashKey;

    int evaluatePiece(uint64_t bb, int val, const int pst[64], bool white) const {
        int s = 0;
        while (bb) {
            int sq = __builtin_ctzll(bb);
            s += val + pst[white ? sq : (sq ^ 56)];
            bb &= (bb - 1);
        }
        return s;
    }

public:
    enum PieceType { NONE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };

    struct SearchLimits {
        int wtime = -1, btime = -1, winc = 0, binc = 0, movestogo = 30;
        int depth = 100, movetime = -1;
        bool infinite = false;
    };

    std::atomic<bool> stopSearch{false};
    chrono::time_point<chrono::steady_clock> startTime;
    long long timeLimit = 0;
    uint64_t nodes = 0;

    void checkTime() {
        if (timeLimit != -1) {
            auto now = chrono::steady_clock::now();
            auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - startTime).count();
            if (elapsed >= timeLimit) stopSearch = true;
        }
    }



    enum HashFlag { HASH_EXACT, HASH_ALPHA, HASH_BETA };
    struct TTEntry {
        uint64_t key;
        int score;
        int depth;
        int flag;
        uint16_t bestMove;
    };
    vector<TTEntry> tt;
    const int TT_SIZE = 1000000;

    void clearTT() {
        for (int i = 0; i < TT_SIZE; i++) tt[i].key = 0;
    }

    int scoreToTT(int score, int ply) {
        if (score > 49000) return score + ply;
        if (score < -49000) return score - ply;
        return score;
    }

    int scoreFromTT(int score, int ply) {
        if (score > 49000) return score - ply;
        if (score < -49000) return score + ply;
        return score;
    }

    int probeTT(uint64_t key, int depth, int alpha, int beta, int ply, uint16_t& bestMove) {
        TTEntry& entry = tt[key % TT_SIZE];
        if (entry.key == key) {
            bestMove = entry.bestMove;
            if (entry.depth >= depth) {
                int score = scoreFromTT(entry.score, ply);
                if (entry.flag == HASH_EXACT) return score;
                if (entry.flag == HASH_ALPHA && score <= alpha) return alpha;
                if (entry.flag == HASH_BETA && score >= beta) return beta;
            }
        }
        return -1000001;
    }

    void storeTT(uint64_t key, int depth, int score, int flag, uint16_t bestMove, int ply) {
        TTEntry& entry = tt[key % TT_SIZE];
        entry.key = key;
        entry.score = scoreToTT(score, ply);
        entry.depth = depth;
        entry.flag = flag;
        entry.bestMove = bestMove;
    }

    uint64_t generateHashKey() {
        uint64_t h = 0;
        uint64_t occupancy = (whitePawn | whiteKing | whiteQueen | whiteBishop | whiteKnight | whiteRook) |
                             (blackPawn | blackKing | blackQueen | blackBishop | blackKnight | blackRook);
        uint64_t temp = occupancy;
        while (temp) {
            int sq = __builtin_ctzll(temp);
            int pt = getPieceAt(sq, true);
            if (pt != NONE) h ^= Evaluation::pieceKeys[pt - 1][sq];
            else {
                pt = getPieceAt(sq, false);
                if (pt != NONE) h ^= Evaluation::pieceKeys[pt + 5][sq];
            }
            temp &= (temp - 1);
        }
        if (!sideToMove) h ^= Evaluation::sideKey;
        h ^= Evaluation::castleKeys[castlingRights];
        if (enPassantSquare != -1) h ^= Evaluation::enPassantKeys[enPassantSquare % 8];
        return h;
    }

    uint64_t getPolyglotHash() const {
        uint64_t h = 0;
        uint64_t occupancy = (whitePawn | whiteKing | whiteQueen | whiteBishop | whiteKnight | whiteRook) |
                             (blackPawn | blackKing | blackQueen | blackBishop | blackKnight | blackRook);
        uint64_t temp = occupancy;
        while (temp) {
            int sq = __builtin_ctzll(temp);
            int pt = getPieceAt(sq, true);
            if (pt != NONE) h ^= Polyglot::RANDOM_ARRAY[64 * (2 * pt - 1) + sq];
            else {
                pt = getPieceAt(sq, false);
                if (pt != NONE) h ^= Polyglot::RANDOM_ARRAY[64 * (2 * pt - 2) + sq];
            }
            temp &= (temp - 1);
        }
        if (castlingRights & WK) h ^= Polyglot::RANDOM_ARRAY[768];
        if (castlingRights & WQ) h ^= Polyglot::RANDOM_ARRAY[769];
        if (castlingRights & BK) h ^= Polyglot::RANDOM_ARRAY[770];
        if (castlingRights & BQ) h ^= Polyglot::RANDOM_ARRAY[771];
        if (enPassantSquare != -1) {
            bool capturePossible = false;
            if (sideToMove) {
                if (enPassantSquare % 8 > 0 && (whitePawn & (1ULL << (enPassantSquare - 9)))) capturePossible = true;
                if (enPassantSquare % 8 < 7 && (whitePawn & (1ULL << (enPassantSquare - 7)))) capturePossible = true;
            } else {
                if (enPassantSquare % 8 > 0 && (blackPawn & (1ULL << (enPassantSquare + 7)))) capturePossible = true;
                if (enPassantSquare % 8 < 7 && (blackPawn & (1ULL << (enPassantSquare + 9)))) capturePossible = true;
            }
            if (capturePossible) h ^= Polyglot::RANDOM_ARRAY[772 + (enPassantSquare % 8)];
        }
        if (sideToMove) h ^= Polyglot::RANDOM_ARRAY[780];
        return h;
    }

    int evaluate() const {
        int score = 0;
        score += evaluatePiece(whitePawn, Evaluation::PAWN_VAL, Evaluation::pawn_pst, true);
        score += evaluatePiece(whiteKnight, Evaluation::KNIGHT_VAL, Evaluation::knight_pst, true);
        score += evaluatePiece(whiteBishop, Evaluation::BISHOP_VAL, Evaluation::bishop_pst, true);
        score += evaluatePiece(whiteRook, Evaluation::ROOK_VAL, Evaluation::rook_pst, true);
        score += evaluatePiece(whiteQueen, Evaluation::QUEEN_VAL, Evaluation::queen_pst, true);
        score += evaluatePiece(whiteKing, Evaluation::KING_VAL, Evaluation::king_pst, true);

        score -= evaluatePiece(blackPawn, Evaluation::PAWN_VAL, Evaluation::pawn_pst, false);
        score -= evaluatePiece(blackKnight, Evaluation::KNIGHT_VAL, Evaluation::knight_pst, false);
        score -= evaluatePiece(blackBishop, Evaluation::BISHOP_VAL, Evaluation::bishop_pst, false);
        score -= evaluatePiece(blackRook, Evaluation::ROOK_VAL, Evaluation::rook_pst, false);
        score -= evaluatePiece(blackQueen, Evaluation::QUEEN_VAL, Evaluation::queen_pst, false);
        score -= evaluatePiece(blackKing, Evaluation::KING_VAL, Evaluation::king_pst, false);

        return sideToMove ? score : -score;
    }

    struct UndoInfo {
        uint8_t castlingRights;
        int enPassantSquare;
        int halfmoveClock;
        int capturedPieceType;
        uint16_t move;
        uint64_t hashKey;
    };

    enum MoveFlags {
        QUIET = 0,
        DOUBLE_PUSH = 1,
        K_CASTLE = 2,
        Q_CASTLE = 3,
        CAPTURE = 4,
        EP_CAPTURE = 5,
        PROMO_KNIGHT = 8,
        PROMO_BISHOP = 9,
        PROMO_ROOK = 10,
        PROMO_QUEEN = 11,
        PROMO_KNIGHT_CAP = 12,
        PROMO_BISHOP_CAP = 13,
        PROMO_ROOK_CAP = 14,
        PROMO_QUEEN_CAP = 15
    };

    static inline int getFrom(uint16_t move) { return move & 0x3F; }
    static inline int getTo(uint16_t move) { return (move >> 6) & 0x3F; }
    static inline int getFlags(uint16_t move) { return (move >> 12) & 0xF; }

    bool getSideToMove() const { return sideToMove; }
    uint8_t getCastlingRights() const { return castlingRights; }
    int getEnPassantSquare() const { return enPassantSquare; }

    int getPieceAt(int square, bool side) const {
        uint64_t bit = (1ULL << square);
        if (side) { // White
            if (whitePawn & bit) return PAWN;
            if (whiteKnight & bit) return KNIGHT;
            if (whiteBishop & bit) return BISHOP;
            if (whiteRook & bit) return ROOK;
            if (whiteQueen & bit) return QUEEN;
            if (whiteKing & bit) return KING;
        } else { // Black
            if (blackPawn & bit) return PAWN;
            if (blackKnight & bit) return KNIGHT;
            if (blackBishop & bit) return BISHOP;
            if (blackRook & bit) return ROOK;
            if (blackQueen & bit) return QUEEN;
            if (blackKing & bit) return KING;
        }
        return NONE;
    }

    bool inCheck(bool side) const {
        uint64_t kingBB = side ? whiteKing : blackKing;
        return (attackedSquares(!side) & kingBB) != 0;
    }

    int getMoveScore(uint16_t move, uint16_t hashMove) const {
        if (move == hashMove) return 2000000;
        int flags = getFlags(move);
        if (flags & CAPTURE) {
            int attacker = getPieceAt(getFrom(move), sideToMove);
            int victim = getPieceAt(getTo(move), !sideToMove);
            if (flags == EP_CAPTURE) victim = PAWN;
            
            auto getVal = [](int t) {
                switch(t) {
                    case PAWN: return 1;
                    case KNIGHT: return 2;
                    case BISHOP: return 3;
                    case ROOK: return 4;
                    case QUEEN: return 5;
                    case KING: return 6;
                    default: return 0;
                }
            };
            return 1000 + (getVal(victim) * 10) - getVal(attacker);
        }
        return 0;
    }

    int quiescence(int alpha, int beta) {
        nodes++;
        if (nodes % 2048 == 0) checkTime();
        if (stopSearch) return 0;

        int standPat = evaluate();
        if (standPat >= beta) return beta;
        if (alpha < standPat) alpha = standPat;

        vector<uint16_t> moves = generateMoves(sideToMove);
        struct ScoredMove { uint16_t move; int score; };
        vector<ScoredMove> scoredMoves;
        for (uint16_t m : moves) {
            if (getFlags(m) & CAPTURE) scoredMoves.push_back({m, getMoveScore(m, 0)});
        }
        sort(scoredMoves.begin(), scoredMoves.end(), [](const ScoredMove& a, const ScoredMove& b) {
            return a.score > b.score;
        });

        for (auto& sm : scoredMoves) {
            UndoInfo undo;
            if (makeMove(sm.move, undo)) {
                int score = -quiescence(-beta, -alpha);
                unmakeMove(sm.move, undo);
                if (stopSearch) return 0;
                if (score >= beta) return beta;
                if (score > alpha) alpha = score;
            }
        }
        return alpha;
    }

    int negamax(int depth, int alpha, int beta, int ply, uint16_t& bestMoveOut, uint16_t pvMove) {
        nodes++;
        if (nodes % 2048 == 0) checkTime();
        if (stopSearch) return 0;

        int oldAlpha = alpha;
        uint16_t ttMove = 0;
        int ttScore = probeTT(hashKey, depth, alpha, beta, ply, ttMove);
        if (ttScore != -1000001) {
            if (ply == 0) bestMoveOut = ttMove;
            return ttScore;
        }

        if (depth <= 0) return quiescence(alpha, beta);

        vector<uint16_t> moves = generateMoves(sideToMove);
        struct ScoredMove { uint16_t move; int score; };
        vector<ScoredMove> scoredMoves;
        uint16_t bestMoveThisNode = ttMove ? ttMove : pvMove;
        for (uint16_t m : moves) scoredMoves.push_back({m, getMoveScore(m, bestMoveThisNode)});
        
        sort(scoredMoves.begin(), scoredMoves.end(), [](const ScoredMove& a, const ScoredMove& b) {
            return a.score > b.score;
        });

        int legalMoves = 0;
        int bestScore = -1000000;
        uint16_t localBestMove = 0;

        for (auto& sm : scoredMoves) {
            UndoInfo undo;
            if (makeMove(sm.move, undo)) {
                legalMoves++;
                
                if (ply == 0 && moveToString(sm.move) == "d4c2") {
                    // cout << "info string Found d4c2 legal at ply 0. Score will be calculated." << endl;
                }

                uint16_t dummy;
                int score = -negamax(depth - 1, -beta, -alpha, ply + 1, dummy, 0);
                unmakeMove(sm.move, undo);

                if (stopSearch) return 0;

                if (score > bestScore) {
                    bestScore = score;
                    localBestMove = sm.move;
                }
                if (score > alpha) {
                    alpha = score;
                }
                if (alpha >= beta) break;
            }
        }

        if (legalMoves == 0) {
            if (inCheck(sideToMove)) return -50000 + ply;
            return 0;
        }

        int flag = HASH_EXACT;
        if (bestScore <= oldAlpha) flag = HASH_ALPHA;
        else if (bestScore >= beta) flag = HASH_BETA;
        storeTT(hashKey, depth, bestScore, flag, localBestMove, ply);

        if (ply == 0) bestMoveOut = localBestMove;
        return bestScore;
    }

    uint16_t search(const SearchLimits& limits) {
        stopSearch = false;
        nodes = 0;
        startTime = chrono::steady_clock::now();
        
        uint64_t polyglotHash = getPolyglotHash();
        uint16_t bookMove = Polyglot::getBookMove(polyglotHash, *this);
        if (bookMove != 0) {
            cout << "info string Found book move (Polyglot)" << endl;
            return bookMove;
        }

        if (limits.movetime != -1) {
            timeLimit = limits.movetime - 50;
        } else if (sideToMove ? (limits.wtime != -1) : (limits.btime != -1)) {
            int time = sideToMove ? limits.wtime : limits.btime;
            int inc = sideToMove ? limits.winc : limits.binc;
            int mtg = (limits.movestogo > 0) ? limits.movestogo : 30;
            timeLimit = time / mtg + inc - 50;
        } else {
            timeLimit = -1;
        }
        if (timeLimit < 20 && timeLimit != -1) timeLimit = 20;

        uint16_t bestMove = 0;
        // Fallback: get the first legal move
        vector<uint16_t> allMoves = generateMoves(sideToMove);
        for (uint16_t m : allMoves) {
            UndoInfo undo;
            if (makeMove(m, undo)) {
                unmakeMove(m, undo);
                bestMove = m;
                break;
            }
        }

        for (int d = 1; d <= limits.depth; d++) {
            uint16_t currentBest = 0;
            int score = negamax(d, -1000000, 1000000, 0, currentBest, bestMove);
            
            if (stopSearch) {
                if (currentBest != 0) bestMove = currentBest;
                break;
            }
            
            if (currentBest != 0) bestMove = currentBest;
            
            auto now = chrono::steady_clock::now();
            auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - startTime).count();
            if (elapsed == 0) elapsed = 1;
            cout << "info depth " << d << " score cp " << score << " nodes " << nodes << " time " << elapsed << " nps " << (nodes * 1000 / elapsed) << " pv " << moveToString(bestMove) << endl;
            
            if (limits.infinite == false && timeLimit != -1 && elapsed > timeLimit / 2) break; 
        }
        return bestMove;
    }

    void togglePiece(int pieceType, int square, bool side) {
        uint64_t bit = (1ULL << square);
        if (side) {
            hashKey ^= Evaluation::pieceKeys[pieceType - 1][square];
            switch (pieceType) {
                case PAWN: whitePawn ^= bit; break;
                case KNIGHT: whiteKnight ^= bit; break;
                case BISHOP: whiteBishop ^= bit; break;
                case ROOK: whiteRook ^= bit; break;
                case QUEEN: whiteQueen ^= bit; break;
                case KING: whiteKing ^= bit; break;
            }
        } else {
            hashKey ^= Evaluation::pieceKeys[pieceType + 5][square];
            switch (pieceType) {
                case PAWN: blackPawn ^= bit; break;
                case KNIGHT: blackKnight ^= bit; break;
                case BISHOP: blackBishop ^= bit; break;
                case ROOK: blackRook ^= bit; break;
                case QUEEN: blackQueen ^= bit; break;
                case KING: blackKing ^= bit; break;
            }
        }
    }

    bool makeMove(uint16_t move, UndoInfo& undo) {
        int from = getFrom(move);
        int to = getTo(move);
        int flags = getFlags(move);
        bool side = sideToMove;

        undo.castlingRights = castlingRights;
        undo.enPassantSquare = enPassantSquare;
        undo.halfmoveClock = halfmoveClock;
        undo.capturedPieceType = NONE;
        undo.move = move;
        undo.hashKey = hashKey;

        int piece = getPieceAt(from, side);
        hashKey ^= Evaluation::castleKeys[castlingRights];
        if (enPassantSquare != -1) hashKey ^= Evaluation::enPassantKeys[enPassantSquare % 8];

        togglePiece(piece, from, side);

        if (flags & CAPTURE) {
            if (flags == EP_CAPTURE) {
                int epSq = side ? to - 8 : to + 8;
                undo.capturedPieceType = PAWN;
                togglePiece(PAWN, epSq, !side);
            } else {
                undo.capturedPieceType = getPieceAt(to, !side);
                if (undo.capturedPieceType != NONE) {
                    togglePiece(undo.capturedPieceType, to, !side);
                }
            }
        }

        if (flags == K_CASTLE) {
            if (side) { togglePiece(ROOK, 7, 1); togglePiece(ROOK, 5, 1); }
            else { togglePiece(ROOK, 63, 0); togglePiece(ROOK, 61, 0); }
        } else if (flags == Q_CASTLE) {
            if (side) { togglePiece(ROOK, 0, 1); togglePiece(ROOK, 3, 1); }
            else { togglePiece(ROOK, 56, 0); togglePiece(ROOK, 59, 0); }
        }

        if (flags >= PROMO_KNIGHT) {
            int promoPiece = KNIGHT;
            int promoType = flags & 3;
            if (promoType == 1) promoPiece = BISHOP;
            else if (promoType == 2) promoPiece = ROOK;
            else if (promoType == 3) promoPiece = QUEEN;
            togglePiece(promoPiece, to, side);
        } else {
            togglePiece(piece, to, side);
        }

        castlingRights &= (castlingMask[from] & castlingMask[to]);
        enPassantSquare = (flags == DOUBLE_PUSH) ? (side ? from + 8 : from - 8) : -1;

        hashKey ^= Evaluation::castleKeys[castlingRights];
        if (enPassantSquare != -1) hashKey ^= Evaluation::enPassantKeys[enPassantSquare % 8];

        halfmoveClock++;
        if (piece == PAWN || (flags & CAPTURE)) halfmoveClock = 0;
        if (!side) fullmoveNumber++;

        sideToMove = !sideToMove;
        hashKey ^= Evaluation::sideKey;

        uint64_t kingBB = side ? whiteKing : blackKing;
        uint64_t attacks = attackedSquares(!side);
        if (attacks & kingBB) {
            unmakeMove(move, undo);
            return false;
        }
        return true;
    }

    void unmakeMove(uint16_t move, const UndoInfo& undo) {
        int from = getFrom(move);
        int to = getTo(move);
        int flags = getFlags(move);
        sideToMove = !sideToMove;
        bool side = sideToMove;

        int piece = getPieceAt(to, side);

        if (flags >= PROMO_KNIGHT) {
            togglePiece(piece, to, side);
            togglePiece(PAWN, to, side);
            piece = PAWN;
        }

        togglePiece(piece, to, side);
        togglePiece(piece, from, side);

        if (flags & CAPTURE) {
            if (flags == EP_CAPTURE) {
                int epSq = side ? to - 8 : to + 8;
                togglePiece(PAWN, epSq, !side);
            } else if (undo.capturedPieceType != NONE) {
                togglePiece(undo.capturedPieceType, to, !side);
            }
        }

        if (flags == K_CASTLE) {
            if (side) { togglePiece(ROOK, 7, 1); togglePiece(ROOK, 5, 1); }
            else { togglePiece(ROOK, 63, 0); togglePiece(ROOK, 61, 0); }
        } else if (flags == Q_CASTLE) {
            if (side) { togglePiece(ROOK, 0, 1); togglePiece(ROOK, 3, 1); }
            else { togglePiece(ROOK, 56, 0); togglePiece(ROOK, 59, 0); }
        }

        castlingRights = undo.castlingRights;
        enPassantSquare = undo.enPassantSquare;
        halfmoveClock = undo.halfmoveClock;
        if (!side) fullmoveNumber--;
        hashKey = undo.hashKey;
    }

    uint16_t package_move(int from, int to, int flags = 0) {
        return (uint16_t)(from | (to << 6) | (flags << 12));
    }

    void resetBoard() {
        whitePawn = 0x000000000000FF00ULL;
        whiteKing = 0x0000000000000010ULL;
        whiteQueen = 0x0000000000000008ULL;
        whiteBishop = 0x0000000000000024ULL;
        whiteKnight = 0x0000000000000042ULL;
        whiteRook = 0x0000000000000081ULL;

        blackPawn = 0x00FF000000000000ULL;
        blackKing = 0x1000000000000000ULL;
        blackQueen = 0x0800000000000000ULL;
        blackBishop = 0x2400000000000000ULL;
        blackKnight = 0x4200000000000000ULL;
        blackRook = 0x8100000000000000ULL;

        castlingRights = 15;
        enPassantSquare = -1;
        sideToMove = 1;
        halfmoveClock = 0;
        fullmoveNumber = 1;
        hashKey = generateHashKey();
    }

    void parseFEN(const string& fen) {
        whitePawn = whiteKnight = whiteBishop = whiteRook = whiteQueen = whiteKing = 0;
        blackPawn = blackKnight = blackBishop = blackRook = blackQueen = blackKing = 0;
        castlingRights = 0;
        enPassantSquare = -1;
        halfmoveClock = 0;
        fullmoveNumber = 1;
        sideToMove = true;

        istringstream ss(fen);
        string pieces, side, castling, ep, halfmove, fullmove;
        if (!(ss >> pieces)) return;
        if (!(ss >> side)) side = "w";
        if (!(ss >> castling)) castling = "-";
        if (!(ss >> ep)) ep = "-";
        if (!(ss >> halfmove)) halfmove = "0";
        if (!(ss >> fullmove)) fullmove = "1";

        int rank = 7, file = 0;
        for (char c : pieces) {
            if (c == '/') { rank--; file = 0; }
            else if (isdigit(c)) { file += (c - '0'); }
            else {
                int square = rank * 8 + file;
                if (c == 'P') whitePawn |= (1ULL << square);
                else if (c == 'N') whiteKnight |= (1ULL << square);
                else if (c == 'B') whiteBishop |= (1ULL << square);
                else if (c == 'R') whiteRook |= (1ULL << square);
                else if (c == 'Q') whiteQueen |= (1ULL << square);
                else if (c == 'K') whiteKing |= (1ULL << square);
                else if (c == 'p') blackPawn |= (1ULL << square);
                else if (c == 'n') blackKnight |= (1ULL << square);
                else if (c == 'b') blackBishop |= (1ULL << square);
                else if (c == 'r') blackRook |= (1ULL << square);
                else if (c == 'q') blackQueen |= (1ULL << square);
                else if (c == 'k') blackKing |= (1ULL << square);
                file++;
            }
        }
        sideToMove = (side == "w");
        for (char c : castling) {
            if (c == 'K') castlingRights |= WK;
            else if (c == 'Q') castlingRights |= WQ;
            else if (c == 'k') castlingRights |= BK;
            else if (c == 'q') castlingRights |= BQ;
        }
        if (ep != "-" && ep.length() >= 2) {
            enPassantSquare = (ep[1] - '1') * 8 + (ep[0] - 'a');
        }
        try {
            halfmoveClock = stoi(halfmove);
            fullmoveNumber = stoi(fullmove);
        } catch (...) {}
        hashKey = generateHashKey();
    }

    uint16_t parseMove(string moveStr) {
        vector<uint16_t> moves = generateMoves(sideToMove);
        for (uint16_t move : moves) {
            if (moveToString(move) == moveStr) {
                UndoInfo undo;
                if (makeMove(move, undo)) {
                    unmakeMove(move, undo);
                    return move;
                }
            }
        }
        return 0;
    }

    void init() {
        for (int i = 0; i < 64; i++) castlingMask[i] = 15;
        castlingMask[0] = 13; castlingMask[7] = 14; castlingMask[4] = 12;
        castlingMask[56] = 7; castlingMask[63] = 11; castlingMask[60] = 3;
        tt.resize(TT_SIZE);
        clearTT();
        resetBoard();
        calculateLeapers();
    }

    void calculateLeapers() {
        const uint64_t notAFile = 0xFEFEFEFEFEFEFEFEULL;
        const uint64_t notHFile = 0x7F7F7F7F7F7F7F7FULL;
        const uint64_t notABFile = 0xFCFCFCFCFCFCFCFCULL;
        const uint64_t notGHFile = 0x3F3F3F3F3F3F3F3FULL;
        for (int square = 0; square < 64; square++) {
            uint64_t startingSquare = 1ULL << square;
            uint64_t kingMoves = 0;
            kingMoves |= (startingSquare << 8) | (startingSquare >> 8);
            kingMoves |= ((startingSquare & notHFile) << 1) | ((startingSquare & notAFile) >> 1);
            kingMoves |= ((startingSquare & notHFile) << 9) | ((startingSquare & notAFile) << 7);
            kingMoves |= ((startingSquare & notHFile) >> 7) | ((startingSquare & notAFile) >> 9);
            kingMoveMask[square] = kingMoves;
            uint64_t knightMoves = 0;
            knightMoves |= ((startingSquare & notHFile) << 17) | ((startingSquare & notGHFile) << 10) | ((startingSquare & notGHFile) >> 6) | ((startingSquare & notHFile) >> 15);
            knightMoves |= ((startingSquare & notAFile) << 15) | ((startingSquare & notABFile) << 6) | ((startingSquare & notABFile) >> 10) | ((startingSquare & notAFile) >> 17);
            knightMoveMask[square] = knightMoves;
            }
            }


    uint64_t getRookMoves(int square, uint64_t occupancy) const {
        uint64_t attacks = 0;
        uint64_t fileA = 0x0101010101010101ULL, fileH = 0x8080808080808080ULL;
        uint64_t rank1 = 0x00000000000000FFULL, rank8 = 0xFF00000000000000ULL;
        int copy = square;
        while (!((1ULL << copy) & rank8)) { copy += 8; attacks |= (1ULL << copy); if ((1ULL << copy) & occupancy) break; }
        copy = square;
        while (!((1ULL << copy) & rank1)) { copy -= 8; attacks |= (1ULL << copy); if ((1ULL << copy) & occupancy) break; }
        copy = square;
        while (!((1ULL << copy) & fileH)) { copy += 1; attacks |= (1ULL << copy); if ((1ULL << copy) & occupancy) break; }
        copy = square;
        while (!((1ULL << copy) & fileA)) { copy -= 1; attacks |= (1ULL << copy); if ((1ULL << copy) & occupancy) break; }
        return attacks;
    }

    uint64_t getBishopAttacks(int square, uint64_t occupancy) const {
        uint64_t attacks = 0;
        uint64_t fileA = 0x0101010101010101ULL, fileH = 0x8080808080808080ULL;
        uint64_t rank1 = 0x00000000000000FFULL, rank8 = 0xFF00000000000000ULL;
        int copy = square;
        while (!((1ULL << copy) & rank8) && !((1ULL << copy) & fileH)) { copy += 9; attacks |= (1ULL << copy); if ((1ULL << copy) & occupancy) break; }
        copy = square;
        while (!((1ULL << copy) & rank8) && !((1ULL << copy) & fileA)) { copy += 7; attacks |= (1ULL << copy); if ((1ULL << copy) & occupancy) break; }
        copy = square;
        while (!((1ULL << copy) & rank1) && !((1ULL << copy) & fileH)) { copy -= 7; attacks |= (1ULL << copy); if ((1ULL << copy) & occupancy) break; }
        copy = square;
        while (!((1ULL << copy) & rank1) && !((1ULL << copy) & fileA)) { copy -= 9; attacks |= (1ULL << copy); if ((1ULL << copy) & occupancy) break; }
        return attacks;
    }

    uint64_t attackedSquares(bool side) const {
        uint64_t attacks = 0;
        uint64_t occupancy = (whitePawn | whiteKing | whiteQueen | whiteBishop | whiteKnight | whiteRook) |
                             (blackPawn | blackKing | blackQueen | blackBishop | blackKnight | blackRook);
        const uint64_t notAFile = 0xFEFEFEFEFEFEFEFEULL, notHFile = 0x7F7F7F7F7F7F7F7FULL;
        if (side) {
            attacks |= (whitePawn << 7) & notHFile;
            attacks |= (whitePawn << 9) & notAFile;
            uint64_t knights = whiteKnight;
            while (knights) { int sq = __builtin_ctzll(knights); attacks |= knightMoveMask[sq]; knights &= (knights - 1); }
            uint64_t diagSliders = whiteBishop | whiteQueen;
            while (diagSliders) { int sq = __builtin_ctzll(diagSliders); attacks |= getBishopAttacks(sq, occupancy); diagSliders &= (diagSliders - 1); }
            uint64_t orthSliders = whiteRook | whiteQueen;
            while (orthSliders) { int sq = __builtin_ctzll(orthSliders); attacks |= getRookMoves(sq, occupancy); orthSliders &= (orthSliders - 1); }
            if (whiteKing) attacks |= kingMoveMask[__builtin_ctzll(whiteKing)];
        } else {
            attacks |= (blackPawn >> 7) & notAFile;
            attacks |= (blackPawn >> 9) & notHFile;
            uint64_t knights = blackKnight;
            while (knights) { int sq = __builtin_ctzll(knights); attacks |= knightMoveMask[sq]; knights &= (knights - 1); }
            uint64_t diagSliders = blackBishop | blackQueen;
            while (diagSliders) { int sq = __builtin_ctzll(diagSliders); attacks |= getBishopAttacks(sq, occupancy); diagSliders &= (diagSliders - 1); }
            uint64_t orthSliders = blackRook | blackQueen;
            while (orthSliders) { int sq = __builtin_ctzll(orthSliders); attacks |= getRookMoves(sq, occupancy); orthSliders &= (orthSliders - 1); }
            if (blackKing) attacks |= kingMoveMask[__builtin_ctzll(blackKing)];
        }
        return attacks;
    }

    void generateRookMoves(uint64_t rookBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveset) {
        uint64_t rank8 = 0xFF00000000000000ULL, rank1 = 0x00000000000000FFULL;
        uint64_t fileH = 0x8080808080808080ULL, fileA = 0x0101010101010101ULL;
        while (rookBoard > 0) {
            int from = __builtin_ctzll(rookBoard);
            int copy = from;
            while (!((1ULL << copy) & rank8)) { copy += 8; if ((1ULL << copy) & friendly) break; int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET; moveset.push_back(package_move(from, copy, flag)); if ((1ULL << copy) & occupied) break; }
            copy = from;
            while (!((1ULL << copy) & rank1)) { copy -= 8; if ((1ULL << copy) & friendly) break; int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET; moveset.push_back(package_move(from, copy, flag)); if ((1ULL << copy) & occupied) break; }
            copy = from;
            while (!((1ULL << copy) & fileH)) { copy += 1; if ((1ULL << copy) & friendly) break; int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET; moveset.push_back(package_move(from, copy, flag)); if ((1ULL << copy) & occupied) break; }
            copy = from;
            while (!((1ULL << copy) & fileA)) { copy -= 1; if ((1ULL << copy) & friendly) break; int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET; moveset.push_back(package_move(from, copy, flag)); if ((1ULL << copy) & occupied) break; }
            rookBoard &= (rookBoard - 1);
        }
    }

    void generateBishopMoves(uint64_t bishopBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveset) {
        uint64_t rank8 = 0xFF00000000000000ULL, rank1 = 0x00000000000000FFULL;
        uint64_t fileH = 0x8080808080808080ULL, fileA = 0x0101010101010101ULL;
        while (bishopBoard > 0) {
            int from = __builtin_ctzll(bishopBoard);
            int copy = from;
            while (!((1ULL << copy) & rank8) && !((1ULL << copy) & fileH)) { copy += 9; if ((1ULL << copy) & friendly) break; int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET; moveset.push_back(package_move(from, copy, flag)); if ((1ULL << copy) & occupied) break; }
            copy = from;
            while (!((1ULL << copy) & fileA) && !((1ULL << copy) & rank8)) { copy += 7; if ((1ULL << copy) & friendly) break; int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET; moveset.push_back(package_move(from, copy, flag)); if ((1ULL << copy) & occupied) break; }
            copy = from;
            while (!((1ULL << copy) & fileH) && !((1ULL << copy) & rank1)) { copy -= 7; if ((1ULL << copy) & friendly) break; int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET; moveset.push_back(package_move(from, copy, flag)); if ((1ULL << copy) & occupied) break; }
            copy = from;
            while (!((1ULL << copy) & fileA) && !((1ULL << copy) & rank1)) { copy -= 9; if ((1ULL << copy) & friendly) break; int flag = ((1ULL << copy) & occupied) ? CAPTURE : QUIET; moveset.push_back(package_move(from, copy, flag)); if ((1ULL << copy) & occupied) break; }
            bishopBoard &= (bishopBoard - 1);
        }
    }

    void generateQueenMoves(uint64_t queenBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveset) {
        generateRookMoves(queenBoard, occupied, friendly, moveset);
        generateBishopMoves(queenBoard, occupied, friendly, moveset);
    }

    void generateKnightMoves(uint64_t knightBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveset) {
        while (knightBoard > 0) {
            int from = __builtin_ctzll(knightBoard);
            uint64_t attacks = knightMoveMask[from] & ~friendly;
            while (attacks) {
                int to = __builtin_ctzll(attacks);
                int flag = ((1ULL << to) & occupied) ? CAPTURE : QUIET;
                moveset.push_back(package_move(from, to, flag));
                attacks &= (attacks - 1);
            }
            knightBoard &= (knightBoard - 1);
        }
    }

    void generateKingMoves(uint64_t kingBoard, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveset) {
        if (kingBoard == 0) return;
        int from = __builtin_ctzll(kingBoard);
        uint64_t attacks = kingMoveMask[from] & ~friendly;
        while (attacks) {
            int to = __builtin_ctzll(attacks);
            int flag = ((1ULL << to) & occupied) ? CAPTURE : QUIET;
            moveset.push_back(package_move(from, to, flag));
            attacks &= (attacks - 1);
        }
    }

    void generateCastlingMoves(bool side, uint64_t occupied, vector<uint16_t>& moveSet) {
        uint64_t attacked = attackedSquares(!side);
        if (side) {
            if ((castlingRights & WK) && !(occupied & ((1ULL << 5) | (1ULL << 6))))
                if (!(attacked & ((1ULL << 4) | (1ULL << 5) | (1ULL << 6)))) moveSet.push_back(package_move(4, 6, K_CASTLE));
            if ((castlingRights & WQ) && !(occupied & ((1ULL << 3) | (1ULL << 2) | (1ULL << 1))))
                if (!(attacked & ((1ULL << 4) | (1ULL << 3) | (1ULL << 2)))) moveSet.push_back(package_move(4, 2, Q_CASTLE));
        } else {
            if ((castlingRights & BK) && !(occupied & ((1ULL << 61) | (1ULL << 62))))
                if (!(attacked & ((1ULL << 60) | (1ULL << 61) | (1ULL << 62)))) moveSet.push_back(package_move(60, 62, K_CASTLE));
            if ((castlingRights & BQ) && !(occupied & ((1ULL << 59) | (1ULL << 58) | (1ULL << 57))))
                if (!(attacked & ((1ULL << 60) | (1ULL << 59) | (1ULL << 58)))) moveSet.push_back(package_move(60, 58, Q_CASTLE));
        }
    }

    void generateWhitePawnMoves(uint64_t whitePawns, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveSet) {
        uint64_t rank2 = 0x000000000000FF00ULL, fileH = 0x8080808080808080ULL, fileA = 0x0101010101010101ULL;
        while (whitePawns > 0) {
            int from = __builtin_ctzll(whitePawns);
            int to = from + 8;
            if (!((1ULL << to) & occupied)) {
                if (to >= 56) { moveSet.push_back(package_move(from, to, PROMO_KNIGHT)); moveSet.push_back(package_move(from, to, PROMO_BISHOP)); moveSet.push_back(package_move(from, to, PROMO_ROOK)); moveSet.push_back(package_move(from, to, PROMO_QUEEN)); }
                else { moveSet.push_back(package_move(from, to, QUIET)); if (((1ULL << from) & rank2) && !((1ULL << (from + 16)) & occupied)) moveSet.push_back(package_move(from, from + 16, DOUBLE_PUSH)); }
            }
            if (!((1ULL << from) & fileA)) { int tl = from + 7; if (((1ULL << tl) & occupied) && !((1ULL << tl) & friendly)) { if (tl >= 56) { moveSet.push_back(package_move(from, tl, PROMO_KNIGHT_CAP)); moveSet.push_back(package_move(from, tl, PROMO_BISHOP_CAP)); moveSet.push_back(package_move(from, tl, PROMO_ROOK_CAP)); moveSet.push_back(package_move(from, tl, PROMO_QUEEN_CAP)); } else moveSet.push_back(package_move(from, tl, CAPTURE)); } else if (tl == enPassantSquare) moveSet.push_back(package_move(from, tl, EP_CAPTURE)); }
            if (!((1ULL << from) & fileH)) { int tr = from + 9; if (((1ULL << tr) & occupied) && !((1ULL << tr) & friendly)) { if (tr >= 56) { moveSet.push_back(package_move(from, tr, PROMO_KNIGHT_CAP)); moveSet.push_back(package_move(from, tr, PROMO_BISHOP_CAP)); moveSet.push_back(package_move(from, tr, PROMO_ROOK_CAP)); moveSet.push_back(package_move(from, tr, PROMO_QUEEN_CAP)); } else moveSet.push_back(package_move(from, tr, CAPTURE)); } else if (tr == enPassantSquare) moveSet.push_back(package_move(from, tr, EP_CAPTURE)); }
            whitePawns &= (whitePawns - 1);
        }
    }

    void generateBlackPawnMoves(uint64_t blackPawns, uint64_t occupied, uint64_t friendly, vector<uint16_t>& moveSet) {
        uint64_t rank7 = 0x00FF000000000000ULL, fileH = 0x8080808080808080ULL, fileA = 0x0101010101010101ULL;
        while (blackPawns > 0) {
            int from = __builtin_ctzll(blackPawns);
            int to = from - 8;
            if (!((1ULL << to) & occupied)) {
                if (to <= 7) { moveSet.push_back(package_move(from, to, PROMO_KNIGHT)); moveSet.push_back(package_move(from, to, PROMO_BISHOP)); moveSet.push_back(package_move(from, to, PROMO_ROOK)); moveSet.push_back(package_move(from, to, PROMO_QUEEN)); }
                else { moveSet.push_back(package_move(from, to, QUIET)); if (((1ULL << from) & rank7) && !((1ULL << (from - 16)) & occupied)) moveSet.push_back(package_move(from, from - 16, DOUBLE_PUSH)); }
            }
            if (!((1ULL << from) & fileA)) { int tl = from - 9; if (((1ULL << tl) & occupied) && !((1ULL << tl) & friendly)) { if (tl <= 7) { moveSet.push_back(package_move(from, tl, PROMO_KNIGHT_CAP)); moveSet.push_back(package_move(from, tl, PROMO_BISHOP_CAP)); moveSet.push_back(package_move(from, tl, PROMO_ROOK_CAP)); moveSet.push_back(package_move(from, tl, PROMO_QUEEN_CAP)); } else moveSet.push_back(package_move(from, tl, CAPTURE)); } else if (tl == enPassantSquare) moveSet.push_back(package_move(from, tl, EP_CAPTURE)); }
            if (!((1ULL << from) & fileH)) { int tr = from - 7; if (((1ULL << tr) & occupied) && !((1ULL << tr) & friendly)) { if (tr <= 7) { moveSet.push_back(package_move(from, tr, PROMO_KNIGHT_CAP)); moveSet.push_back(package_move(from, tr, PROMO_BISHOP_CAP)); moveSet.push_back(package_move(from, tr, PROMO_ROOK_CAP)); moveSet.push_back(package_move(from, tr, PROMO_QUEEN_CAP)); } else moveSet.push_back(package_move(from, tr, CAPTURE)); } else if (tr == enPassantSquare) moveSet.push_back(package_move(from, tr, EP_CAPTURE)); }
            blackPawns &= (blackPawns - 1);
        }
    }

    vector<uint16_t> generateMoves(bool side) {
        uint64_t allWhite = whitePawn | whiteKing | whiteQueen | whiteBishop | whiteKnight | whiteRook;
        uint64_t allBlack = blackPawn | blackKing | blackQueen | blackBishop | blackKnight | blackRook;
        uint64_t occupied = allWhite | allBlack;
        vector<uint16_t> moveSet;
        if (side) {
            generateRookMoves(whiteRook, occupied, allWhite, moveSet);
            generateBishopMoves(whiteBishop, occupied, allWhite, moveSet);
            generateQueenMoves(whiteQueen, occupied, allWhite, moveSet);
            generateKnightMoves(whiteKnight, occupied, allWhite, moveSet);
            generateKingMoves(whiteKing, occupied, allWhite, moveSet);
            generateWhitePawnMoves(whitePawn, occupied, allWhite, moveSet);
            generateCastlingMoves(side, occupied, moveSet);
        } else {
            generateRookMoves(blackRook, occupied, allBlack, moveSet);
            generateBishopMoves(blackBishop, occupied, allBlack, moveSet);
            generateQueenMoves(blackQueen, occupied, allBlack, moveSet);
            generateKnightMoves(blackKnight, occupied, allBlack, moveSet);
            generateKingMoves(blackKing, occupied, allBlack, moveSet);
            generateBlackPawnMoves(blackPawn, occupied, allBlack, moveSet);
            generateCastlingMoves(side, occupied, moveSet);
        }
        return moveSet;
    }

    string squareToCoord(int sq) {
        string s = ""; s += (char)('a' + (sq % 8)); s += (char)('1' + (sq / 8)); return s;
    }

    string moveToString(uint16_t move) {
        int from = getFrom(move), to = getTo(move), flags = getFlags(move);
        string s = squareToCoord(from) + squareToCoord(to);
        if (flags >= PROMO_KNIGHT) {
            int pt = flags & 3;
            if (pt == 0) s += 'n'; else if (pt == 1) s += 'b'; else if (pt == 2) s += 'r'; else if (pt == 3) s += 'q';
        }
        return s;
    }

    uint64_t perft(int depth) {
        if (depth == 0) return 1ULL;
        uint64_t nodes = 0;
        vector<uint16_t> moves = generateMoves(sideToMove);
        for (uint16_t move : moves) {
            UndoInfo undo;
            if (makeMove(move, undo)) { nodes += perft(depth - 1); unmakeMove(move, undo); }
        }
        return nodes;
    }

    void perftDivide(int depth) {
        uint64_t totalNodes = 0;
        vector<uint16_t> moves = generateMoves(sideToMove);
        for (uint16_t move : moves) {
            UndoInfo undo;
            if (makeMove(move, undo)) { uint64_t nodes = perft(depth - 1); unmakeMove(move, undo); cout << moveToString(move) << ": " << nodes << endl; totalNodes += nodes; }
        }
        cout << "\nTotal nodes at depth " << depth << ": " << totalNodes << endl;
    }
};

void uciLoop(Board& myBoard) {
    string command;
    thread searchThread;
    auto stopSearching = [&]() {
        myBoard.stopSearch = true;
        if (searchThread.joinable()) searchThread.join();
    };

    while (getline(cin, command)) {
        istringstream ss(command);
        string token;
        ss >> token;
        if (token == "uci") {
            stopSearching();
            cout << "id name GrandmasterZero" << endl;
            cout << "id author Arnav" << endl;
            cout << "uciok" << endl;
        } else if (token == "isready") {
            cout << "readyok" << endl;
        } else if (token == "ucinewgame") {
            stopSearching();
            myBoard.clearTT();
            myBoard.parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        } else if (token == "position") {
            stopSearching();
            string sub;
            ss >> sub;
            if (sub == "startpos") {
                myBoard.parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
                ss >> sub;
            } else if (sub == "fen") {
                string fenStr = "";
                while (ss >> sub && sub != "moves") fenStr += sub + " ";
                myBoard.parseFEN(fenStr);
            }
            if (sub == "moves") {
                string moveStr;
                while (ss >> moveStr) {
                    uint16_t move = myBoard.parseMove(moveStr);
                    if (move) { 
                        Board::UndoInfo undo; 
                        myBoard.makeMove(move, undo); 
                    } else {
                        break;
                    }
                }
            }
        } else if (token == "perft") {
            stopSearching();
            int depth; if (ss >> depth) myBoard.perftDivide(depth);
        } else if (token == "eval") {
            cout << "Evaluation: " << myBoard.evaluate() << " centipawns" << endl;
        } else if (token == "go") {
            stopSearching();
            Board::SearchLimits limits;
            string sub;
            while (ss >> sub) {
                if (sub == "wtime") ss >> limits.wtime;
                else if (sub == "btime") ss >> limits.btime;
                else if (sub == "winc") ss >> limits.winc;
                else if (sub == "binc") ss >> limits.binc;
                else if (sub == "movestogo") ss >> limits.movestogo;
                else if (sub == "depth") ss >> limits.depth;
                else if (sub == "movetime") ss >> limits.movetime;
                else if (sub == "infinite") limits.infinite = true;
            }
            myBoard.stopSearch = false;
            searchThread = thread([&myBoard, limits]() {
                uint16_t bestMove = myBoard.search(limits);
                cout << "bestmove " << myBoard.moveToString(bestMove) << endl;
            });
        } else if (token == "stop") {
            stopSearching();
        } else if (token == "quit") {
            stopSearching();
            break;
        }
    }
    stopSearching();
}

#ifndef UNIT_TEST
int main() {
    Evaluation::initZobrist();
    Board myBoard;
    myBoard.init();
    uciLoop(myBoard);
    return 0;
}
#endif

namespace Polyglot {
    uint16_t getBookMove(uint64_t key, const Board& board) {
        ifstream file("gm_theory.bin", ios::binary);
        if (!file.is_open()) return 0;
        
        file.seekg(0, ios::end);
        long size = file.tellg();
        int numEntries = size / 16;
        if (numEntries == 0) return 0;

        int low = 0, high = numEntries - 1;
        int firstIdx = -1;

        while (low <= high) {
            int mid = low + (high - low) / 2;
            file.seekg(mid * 16, ios::beg);
            uint64_t midKey;
            file.read((char*)&midKey, 8);
            midKey = swap64(midKey);

            if (midKey >= key) {
                if (midKey == key) firstIdx = mid;
                high = mid - 1;
            } else {
                low = mid + 1;
            }
        }

        if (firstIdx == -1) return 0;

        struct BookMove { uint16_t polyglotMove; uint16_t weight; };
        vector<BookMove> moves;
        uint64_t totalWeightSq = 0;

        file.seekg(firstIdx * 16, ios::beg);
        while (true) {
            Entry e;
            if (!file.read((char*)&e, 16)) break;
            uint64_t entryKey = swap64(e.key);
            if (entryKey != key) break;

            uint16_t m = swap16(e.move);
            uint16_t w = swap16(e.weight);
            moves.push_back({m, w});
            totalWeightSq += (uint64_t)w * w;
        }

        if (moves.empty() || totalWeightSq == 0) return 0;

        uint64_t r = Evaluation::random64() % totalWeightSq;
        uint64_t currentSum = 0;
        uint16_t selectedPolyglotMove = 0;
        for (auto& bm : moves) {
            currentSum += (uint64_t)bm.weight * bm.weight;
            if (r < currentSum) {
                selectedPolyglotMove = bm.polyglotMove;
                break;
            }
        }

        int from_f = (selectedPolyglotMove >> 6) & 7;
        int from_r = (selectedPolyglotMove >> 9) & 7;
        int to_f = selectedPolyglotMove & 7;
        int to_r = (selectedPolyglotMove >> 3) & 7;
        int promo = (selectedPolyglotMove >> 12) & 7;

        int from = from_r * 8 + from_f;
        int to = to_r * 8 + to_f;
        
        vector<uint16_t> legalMoves = const_cast<Board&>(board).generateMoves(board.getSideToMove());
        for (uint16_t m : legalMoves) {
            if (Board::getFrom(m) == from && Board::getTo(m) == to) {
                int m_flags = Board::getFlags(m);
                if (promo == 0) {
                    if (m_flags < 8) return m;
                } else {
                    int m_promo = (m_flags & 3) + 1;
                    if (m_promo == promo) return m;
                }
            }
        }
        return 0;
    }
}
