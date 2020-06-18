#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <syslog.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/wait.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <curl/curl.h>
#include <json.h>
#include <typedefs.h>
#include <bcmutils.h>
#include <shutils.h>
#include <bcmnvram.h>
#include "bcmnvram_f.h"
#include <shared.h>
#include <rtstate.h>
#include <wlioctl.h>
#include <wlutils.h>
#include <auth_lite.h>

#define SI_WL_QUERY_ASSOC 1
#define SI_WL_QUERY_AUTHE 2
#define SI_WL_QUERY_AUTHO 3

static struct itimerval itv;
char arpbuffer[4096];
int swmode, timer, timers;
//0=default
char deflogo[]="";
char OnePlus[] ="C0EEFB,94652D";
char shu360[] ="D0FA1D,74AC5F";
char Asus[] ="00E018,000C6E,001BFC,001E8C,0015F2,002354,001FC6,F832E4,382C4A,60A44C,704D7B,00112F,0011D8,001731,0018F3,485B39,F46D04,3085A9,1CB72C,40167E,E03F49,50465D,D017C2,14DAE9,000EA6,0013D4,002618,00248C,BCEE7B,08606E,C86000,38D547,2C4D54,90E6BA,BCAEC5,10BF48,14DDA9,086266,AC9E17,F07959,54A050,10C37B,7824AF,D850E6,74D02B,9C5C8E,708BCD,2C56DC,001A92,001D60,002215,20CF30,E0CB4E,1C872C,305A3A,AC220B,5404A6,3497F6,00040F";
char Coolpad[] ="54DC1D,EC5A86,D03742,00166D,3C9157,18DC56";
char Dell[] ="ECF4BB,D067E5,18A99B,F8DB88,18FB7B,74E6E2,109836,44A842,34E6D7,000BDB,001143,00188B,D4BED9,001AA0,002170,0026B9,A4BADB,001E4F,5CF9DD,00065B,801844,484D7E,14B31F,180373,F8B156,1C4024,F8BC12,D0431E,246E96,204747,4C7625,B8AC6F,001EC9,7845C4,149ECF,D481D7,989096,B82A72,000F1F,14FEB5,0015C5,D4AE52,549F35,64006A,B4E10F,0023AE,1866DA,28F10E,9840BB,18DBF2,848F69,90B11C,F8CAB8,24B6FD,000D56,00123F,001372,74867A,3417EB,0019B9,002219,00B0D0,5C260A,B083FE,141877,0024E8,F48E38,847BEB,782BCB,B8CA3A,F01FAF,C81F66,001422,001C23,00219B,000874,002564,842B2B,E0DB55,A41F72,00C04F,F04DA2,BC305B,001D09";
char Haier[] ="C8D779,DC330D,BC8AE8,BC2B6B,00258D,0007A8,B0A37E";
char Hasee[] ="0022C0";
char Honor[] ="";
char HP[] ="C09134,0016B9,0024A8,001B3F,002347,002561,F06281,0026F1,B439D6,001FFE,0021F7,001C2E,001F28,001DB3";
char HTC[] ="807ABF,90E7C4,7C6193,2C8A72,980D2E,A826D9,D4206D,188796,002376,04C23E,00EEBD,AC3743,00092D,F8DB7F,E899C4,800184,502E5C,847A88,38E7D8,D8B377,B4CEF6,D40B1A,1CB094,A0F450,404E36,902155,64A769,BCCFCC";
char Huawei[] ="F00FEC,989C57,14D169,044F4C,E4A7C5,F4BF80,AC075F,AC9232,74D21D,48AD08,2CAB00,00E0FC,24DF6A,009ACD,8038BC,D440F0,64A651,E8CD2D,ACE215,EC233D,78F5FD,80B686,10C61F,8853D4,0C37DC,BC7670,24DBAC,0C45BA,CCA223,E8088B,60E701,AC853D,74882A,78D752,E0247F,00464B,707BE8,548998,0819A6,3CF808,B41513,283152,DCD2FC,285FDB,404D8E,781DBA,001E10,D03E5C,F898B9,2CCF58,E4C2D1,88A2D7,3C4711,D0D04B,9C52F8,900325,DC094C,DCEE06,58605F,8828B3,C4F081,801382,483C0C,2C9D1E,C88D83,F04347,9CB2B2,84BE52,F09838,18DED7,C80CC8,0425C5,DCC64B,043389,A0A33B,A8CA7B,ACCF85,EC4D47,88CF98,D8490B,888603,F8E811,E09796,CCCC81,101B54,7054F5,D07AB5,C40528,3CDFBD,14B968,80717A,F49FF3,84DBAC,C07009,E0191D,B8BC1B,241FA0,50A72B,C85195,00F81C,F4559C,283CE4,08E84F,04BD70,18C58A,04C06F,5C4CA9,4C5499,00259E,001882,D494E8,4455B1,30F335,745AAA,7C1CF1,74A528,94DBDA,384C4F,E4A8B6,244C07,94FE22,F823B2,DCD916,08C021,48435A,9CE374,049FCA,C81FBE,203DB2,48D539,14A51A,B08900,8C0D76,005A13,A4C64F,487B6B,80D4A5,38BC01,04B0E7,446A2E,2C55D3,F44C7F,143004,4CFB45,A4BA76,7CB15D,34CDBE,D46AA8,5439DF,4846FB,200BC7,104780,88E3AB,00664B,68A0F6,5CF96A,B43052,88CEFA,582AF7,F48E92,40CBA8,087A4C,D46E5C,2469A5,C8D15E,F83DFF,308730,002568,30D17E,9C28EF,7C6097,60DE44,3400A3,643E8C,E03676,7CA23E,E8BDD1,FCE33C,249EAB,74A063,14D11F,54511B,E47E66,9C741A,E0A3AC,042758,5CA86A,48FD8E,244427,18D276,4CF95D,8421F1,707990,3CFA43,145F94,C0BFC0,A08CF8,C894BB,D0FF98,5004B8,10B1F8,F87588,BC3F8F,6416F0,48DB50,2400BA,A08D16,E84DD0,240995,247F3C,1C8E5C,94772B,F4E3FB,04021F,0034FE,D02DB3,086361,F80113,70723C,5C7D5E,4C8BEF,20F3A3,ACE87B,688F84,ECCB30,786A89,2008ED,509F27,CC96A0,54A51B,F4C714,286ED4,04F938,FC48EF,80FB06,D4B110,CC53B5,EC388F,BC9C31,E435C8,F8BF09,5CB395,9C37F4,BC620E,78F557,E02861,C4473F,D4612E,1C6758,BC7574,20A680,34A2A2,20F17C,34B354,749D8F,346AC2,50016B,307496,708A09,149D09,F4CB52,446EE5,A8C83A,849FB5,9C7DA3,F02FA7,883FD3,341E6B,B0E5ED,C81451,C486E9,D8C771,F0C850,5425EA,68CC6E,7C7D3D,4482E5,B05B67,38F889,F4DCF9,904E2B,0C96BF,9CC172,70A8E3,F84ABF,4CB16C,4C1FCC,486276,AC4E91,E468A3,80D09B,581F28,8C34FD,90671C,587F66,BC25E0,C4072F,0CD6BD,A49947,346BD3,1C1D67,84A8E4,202BC1,105172,9017AC,94049C,68A828,A4DCBE,6889C1,845B12,D4F9A1,5CB43E,04FE8D,480031,98E7F5,24BCF8,FC3F7C,608334,84AD58,50680A,600810,AC6175,7C11CB,A4CAA0,5001D9,44C346,884477,047503,886639,A47174,D4A148,D065CA,8CEBC6,B808D7,407D0F";
char Apple[] ="00CDFE,18AF61,CC4463,6C72E7,CCC760,087402,285AEB,28F076,44D884,EC852F,286ABA,705681,7CD1C3,F0DCE2,B065BD,A82066,BC6778,68967B,848506,B4F0AB,10DDB1,04F7E4,34C059,F0D1A9,F82793,ACFDEC,D0E140,8C7C92,7831C1,F437B7,54AE27,6476BA,84B153,783A84,2CBE08,24E314,60FEC5,00A040,BC3BAF,786C1C,041552,38484C,701124,C86F1D,685B35,380F4A,3010E4,04DB56,881FA1,04E536,109ADD,40A6D9,7CF05F,A4B197,0C74C2,403004,4860BC,50EAD6,28E02C,60C547,7C11BE,003EE1,68D93C,2CF0EE,84788B,6C94F8,703EAC,C01ADA,34363B,C81EE7,9CFC01,000D93,001CB3,64B9E8,34159E,58B035,F0B479,ACBC32,AC61EA,38B54D,A4F1E8,90C1C6,70A2B3,4C57CA,68FB7E,9CF48E,FCD848,70700D,0CD746,440010,E498D6,606944,0452F3,241EEB,F431C3,64A5C3,CC08E0,5855CA,8C7B9D,88C663,C82A14,9803D8,8C5877,3451C9,E0B9BA,D023DB,B88D12,B817C2,68A86D,78A3E4,BC926B,0050E4,003065,000A27,001451,0019E3,002312,002332,002436,00254B,0026BB,E80688,985AEB,2078F0,78D75F,E0ACCB,98E0D9,C0CECD,70E72C,D03311,C8B5B7,A8BBCF,90B21F,B8E856,1499E2,B418D1,80006E,60D9C7,C8F650,1C1AC0,E06678,5C8D4E,64A3CB,44FB42,F41BA1,3CE072,E88D28,CC785F,AC3C0B,88CB87,EC3586,F0C1F1,F4F951,18AF8F,C0F2FB,00F76F,AC87A3,48437C,34A395,9CF387,A85B78,908D6C,0C1539,BC4CC4,0CBC9F,A45E60,680927,60FACD,1CABA7,8CFABA,5C95AE,E0C97A,BC52B7,14109F,542696,D8D1CB,544E90,5CADCF,006D52,1C9E46,E0C767,A860B6,24F094,90B0ED,C4B301,E05F45,483B38,404D7F,7C04D0,BC9FEF,8866A5,784F43,88E87F,B853AC,2C3361,5CF938,3871DE,BC5436,00C610,70DEE2,182032,6CC26B,1040F3,FC253F,183451,C0847A,64200C,74E1B6,0C771A,00F4B9,C8334B,B8F6B1,C09F42,189EFC,6C3E6D,7CFADF,101C0C,001124,001D4F,001E52,001F5B,001FF3,0021E9,00236C,002500,60FB42,F81EDF,90840D,D8A25E,C8BCC8,28E7CF,D89E3F,040CCE,A4D1D2,406C8F,649ABE,94E96A,AC293A,10417F,7014A6,A8667F,D02598,CC29F5,6C709F,0C3E9F,34E2FD,609217,8863DF,80E650,006171,90FD61,5C97F3,6C4008,24A074,F02475,20A2E4,5CF5DA,28E14C,54E43A,C8E0EB,A88808,907240,0C4DE9,D89695,0C3021,F0F61C,B03495,848E0C,8C2DAA,444C0C,84FCFE,E48B7F,5C969D,A8FAD8,949426,E0F5C6,B844D9,DC2B2A,9C4FDA,1C5CF2,E49A79,28A02B,B44BD2,B48B19,ECADB8,9801A7,2CF0A2,609AC1,F07960,9C8BA0,4C3275,E4E4AB,58404E,DC0C5C,2C200B,6C8DC1,38CADA,68DBCA,044BED,A4D18C,CC25EF,7CC537,70CD60,24AB81,581FAA,A46706,3C0754,E4CE8F,E8040B,B8C75D,403CFC,286AB8,7CC3A1,B8782E,000502,0010FA,000393,0016CB,0017F2,001B63,001EC2,002608,7C6D62,40D32D,D83062,C42C03,789F70,DC3714,40331A,94F6A3,D81D72,70ECE4,38C986,FCFC48,A4C361,AC7F3E,280B5C,90B931,24A2E1,80EA96,600308,04F13E,54724F,48746E,3CAB8E,7C6DF8,48D705,3CD0F8,98D6BB,4CB199,64E682,804971,98FE94,D8004D,98B8E3,80929F,885395,9C04EB,78FD94,C88550,D4F46F,787E61,60F81D,4C7C5F,48E9F1,FCE998,F099BF,68644B,A8968A,4C8D79,207D74,F4F15A,042665,2CB43A,689C70,087045,209BCD,F0B0E7,CC20E8,F45C89,BCEC5D,DC415F,30636B,0C5101,086D41,04D3CF,203CAE,748D08,A03BE3,64B0A6,84FCAC,6C19C0,20AB37,186590,28ED6A,34AB37,60A37D,0056CD,7081EB,086698,9060F1,741BB2,28CFE9,E425E7,080007,000A95,002241,0023DF,0025BC,00264A,0026B0,041E64,D49A20,9027E4,60334B,A43135,9C35EB,507A55,A0999B,24240E,903C92,341298,9C293F,A88E24,E8802E,68AE20,E0B52D,80BE05,D8BB2C,D04F7E,2C1F23,549F13,B8098A,F0DBE2,18EE69,748114,18F643,D0A637,A01828,D0034B,5C5948,78CA39,18E7F4,B8FF61,DC2B61,1093E9,442A60,E0F847,145A05,28CFDA,148FC6,283737,045453,F0CBA1,C06394,8C006D,B09FBA,DC86D8,8C2937,DC9B9C,98F0AB,F0DBF8,ACCF5C,3C15C2,04489A,D8CF9C,30F7C5,008865,40B395,3090AB,1CE62B,A0EDCD,A886DD,54EAA8,E4C63D,843835,7073CB,9C207B,842999,74E2F5,20C9D0,7C0191,70480F,A4B805,587F57,80D605,C869CD,BC6C21,0469F8,20768F,C0CCF8,80ED2C,E8B2AC,8489AD,8C8EF2,F40F24,84A134,1C9148,5CF7E6,A0D795,CC088D,B0702D,D0C5F3,60F445,00B362,F86214,70F087,BCA920";
char Lenovo[] ="00061B,207693,A03299,6099D1,A48CDB,98FFD0,503CC4,70720D,D4223F,1436C6,6C5F1C,149FE8,AC3870,CC07E4,C8DDC9,80CF41,EC89F5,0012FE,60D9A0,88708C,D87157,74042B,E02CB2,005907,00061B,D89A34";
char LeEco[] ="LeEco";
char LG[] ="0005C9,E8F2E2,944444,A06FAA,C80210,1C08C1,001EB2,30A9DE,C4366C,5C17D3,8C541D,64995D,B08991,AC0D1B,60E3AC,0C4885,88C9D0,700514,E892A4,A816B2,C4438F,2021A5,6CD68A,001E75,0026E2,001FE3,2C54CF,F895C7,DC0B34,2C598A,88074B,10683F,A039F7,64BC0C,64899A,58A2B5,74A722,001F6B,78F882,0021FB,8C3AE3,30766F,F80CF3,D013FD,0034DA,583F54,001C62,002483,40B0FA,A09169,34FCEF,485929,505527,98D6F7,A8922C,5C70A3,F8A9D0,CCFA00,BCF5AC,00AA70,F01C13,344DF7,C49A02,0022A9,0025E5,10F96F,805A04,5CAF06,B81DAA,3CBDD8,E85B5B,C041F6,9893CC,3CCD93,CC2D8C,B0989F,B40EDC,B061C7,0019A1,001256,3CE624";
char Meitu[] ="C09A71";
char Meizu[] ="683E34,90F052,38BC1A,541473";
char OPPO[] ="BC3AEA,E8BBA8,8C0EE3,6C5C14,2C5BB8,B0AA36,1C48CE,A81B5A,DC6DCD,440444,C09F05,CC2D83,A43D78,EC01EE,A09347,C8F230,1C77F6,E44790";
char Phicomm[] ="CC81DA,006B8E,F0EBD0,D842AC,8CAB8E";
char Samsung[] ="4C6641,88329B,BC8CCD,400E85,EC9BF3,F8042E,843838,54880E,1449E0,C0BDD1,E8508B,F025B7,C8BA94,EC1F72,8CF5A3,AC5F3E,C09727,F409D8,B479A7,2C0E3D,3423BA,D022BE,D02544,FC0012,000918,0023C2,745F00,382DE8,D087E2,205531,5440AD,842E27,50F0D3,84119E,08ECA9,10D38A,382DD1,E0CBEE,64B853,988389,244B03,FC8F90,1816C9,F4428F,188331,8455A5,A87C01,C01173,BCE63F,B857D8,94B10A,E458B8,088C2C,B86CE8,9C65B0,C8A823,C44202,D059E4,64B310,78ABBB,14BB6E,9476B7,8C1ABF,B47443,30CBF8,182195,A88195,88ADD2,1C3ADE,D0FCCC,849866,E89309,0016DB,5C3C27,10D542,A0821F,C45006,BC79AD,30D6C9,B0DF3A,805719,78A873,041BBA,08FD0E,08D42B,00E3B2,C81479,F0728C,94350A,001FCD,D0DFC7,1C62B8,18E2C2,001A8A,002567,A8F274,001599,0012FB,7CF854,8CC8CD,E81132,A02195,8C71F8,04180F,9463D1,0CDFA4,CC051B,68EBAE,60D0A9,60A10A,A07591,001FCC,EC107B,A01081,F83F51,D8E0E1,F05A09,503275,28CC01,B46293,04FE31,845181,D831CF,F8D0BD,FCC734,E4B021,B0EC71,3CBBFD,24F5AA,2CAE2B,C488E5,7C9122,E8B4C8,18895B,E0DB10,E09971,6077E2,680571,6C2F2C,5056BF,9C2A83,E45D75,F877B8,E4FAED,54F201,A06090,141F78,006F64,DC6672,001E7D,3C6200,0024E9,002399,E4E0C5,E8039A,C4731E,8C7712,2013E0,0007AB,0021D2,BC4760,D0176A,2CBABA,24920E,40D3AE,24DBED,AC3613,9852B1,1489FD,CCFE3C,789ED0,E440E2,1CAF05,E492FB,0073E0,BC4486,380B40,002490,0023D7,FCA13E,A00798,945103,C819F7,2C4401,682737,30D587,002538,389496,0CB319,08EE8B,84A466,981DFA,FCF136,0C8910,54FA3E,A89FBA,FC1910,083D88,5C2E59,646CB2,F884F2,14B484,608F5C,4CBCA5,78595E,B0D09C,4CA56D,A48431,E4F8EF,1432D1,E458E7,8CBFA6,7840E4,9000DB,183A2D,08373D,50F520,A4EBD3,28987B,1867B0,F40E22,9C3AAF,0821EF,A0CBFD,34145F,CCB11A,509EA7,DCCF96,B8BBAF,60C5AD,D85B2A,ACC33A,0017D5,001247,E4121D,684898,002339,D487D8,184617,5001BB,380A94,D857EF,1C66AA,58C38B,001EE2,001C43,001D25,3C5A37,549B12,3C8BFE,00265D,D4E8B2,0808C2,B0C4E7,D890E8,34AA8B,24C696,181EB0,20D390,343111,34BE00,78521A,7825AD,F4D9FB,0017C9,00166B,00166C,E47CF9,002454,20D5BF,30CDA7,C87E75,00233A,60A4D0,ACEE9E,C08997,2827BF,F05B7B,7CF90E,AC5A14,B0C559,BCD11F,A0B4A5,80656D,48137E,E83A12,9C0298,6C8336,B8C68E,74458A,A49A58,B4EF39,14A364,3CA10D,206E9C,183F47,0C715D,0C1420,A80600,6CF373,90F1AA,C4576E,78BDBC,78C3E9,C83870,288335,44783E,202D07,98398E,348A7B,BC765E,78009E,F8E61A,888322,84B541,0015B9,001DF6,ECE09B,606BBD,0000F0,4844F7,1C5A3E,F47B5E,008701,FC4203,1C232C,F06BCA,BC20A4,14F42A,BC851F,B85E7B,C462EA,0023D6,002491,001B98,44F459,34C3AC,94D771,4C3C16,9401C2,B43A28,D0C1B1,F008F1,78471D,3816D1,D48890,002566,00265F,D47AE2,9CD35B,60AF6D,B85A73,103047,109266,B047BF,7C0BC6,804E81,244B81,50A4C8,8425DB,D8C4E9,50C8E5,446D6C,38D40B,647791,781FDB,08FC88,30C7AE,18227E,00F46F,BC1485,9CE6E7,380195,5CF6DC,1077B1,508569,40163B,70288B,4849C7,205EF7,182666,C06599,CC07AB,E84E84,50FC9F,E432CB,889B39,BCB1F3,38ECE4,CCF9E8,F0E77E,5CE8EB,B8D9CE,70F927,301966,28BAB5,103B59,6CB7F4,001EE1,0018AF,BC72B1,78F7BE,F49F54,00214C,001632,D0667B,001377,50B7C3,8018A7,444E1A,E8E5D6,5492BE,101DC0,0021D1,5C497D,20DBAB,5C9960,E47DBD,00E064,842519,000DAE,840B2D,000278,78D6F0,B407F9,90187C,FC1F19,50CCF8,980C82,002119,5C0A5B,CC3A61,DC7144,A00BBA,38AA3C,206432,002637,5CA39D,000DE5,FCDBB3";
char Smartisan[] ="B40B44";
char Sony[] ="001315,001FA7,A8E3EE,709E29,FC0FE6,F8461C,BC60A7,0CFE45,F8D0AC,00D9D1,00041F,001D0D,280DFC,0015C1,0019C5,00248D,0022A6,0013A9,AC9B0A,045D4B,5453ED,F0BF97,001A80,0024BE,104FA8,544249,001DBA,78843C,080046,3C0771,D8D43C,30F9ED,00014A,FCF152,BC6E64,A0E453,1C7B21,2421AB,6C23B9,58170C,B8F934,205476,303926,00EB2D,00219E,001E45,001813,4040A7,40B837,C43ABE,307512,4C21D0,402BA1,0025E7,D05162,94CE2C,001620,0012EE,84C7EA,44746C,30A8DB,000AD9,000FDE,001EDC,001963,001B59,0023F1,3017C8,18002D,584822,68764F,0024EF,6C0E0D,B4527D,E063E5,000E07,001A75,0016B8,001D28,001FE4,002298,848EDF,44D4E0,B4527E,001CA4,002345,8C6422,90C115,8400D2,5CB524,9C5CF9";
char TCL[] ="000E1F,001C50,5C36B8,6C5AB5,408BF6,3C591E,4C14A3";
char ThinkPad[] ="";
char TongfangPC[] ="";
char VIVO[] ="101212,28FAA0,3CA348,F42981,3CB6B7,5419C8,10F681,F01B6C,DC1AC5,9CFBD5,205D47,ECDF3A,E45AA2,C46699,FC1A11,E0DDC0,886AB1,18E29F,0823B2,9CA5C0,4813F3";
char Microsoft[] ="38F23E,38256B,B4E1C4,300D43,6C2779,607EDD,6C8FB5,6C2483,A4516F,E498D1,5CCA1A,485073,74E28C,8463D6,D48F33,00155D,985FD3,BC8385,9C6C15,4886E8,2C2997,102F6B,281878,C83F26,F01DBC,80C5E6,206274,0025AE,00125A,0C413E,D0929E,B84FD5,EC59E7,002248,5CBA37,3C8375,149A10,0CE725,20A99B,001DD8,0017FA,0003FF,949AA9,6045BD,7CED8D,5882A8,B4AE2B,3059B7,501AC5,C0335E,4C0BBE,7C1E52,0050F2,000D3A";
char XiaoMi[] ="2047DA,286C07,3CBD3E,9C99A0,185936,98FAE3,640980,8CBEBE,F8A45F,28E31F,0C1DAF,14F65A,742344,F0B429,D4970B,64CC2E,B0E235,38A4ED,F48B32,009EC8,ACF7F3,102AB3,584498,A086C6,7C1DD9,ACC1EE,7802F8,68DFDD,C46AB7,FC64BA,2082C0,3480B3,7451BA,64B473,C40BCB";
char ZTE[] ="74A78E,84742A,681AB2,6CA75F,709F2D,EC1D7F,34DE34,4CCBF5,78E8B6,B49842,8CE081,DC028E,48282F,343759,B805AB,74B57E,540955,981333,F8A34F,C87B5B,98F537,001E73,0019C6,0015EB,98F428,54BE53,300C23,1844E6,38D82F,D87495,344DEA,2C957F,146080,986CF5,CC7B35,F8DFA8,34E0CF,D476EA,789682,749781,E07C13,689FF0,18686A,344B50,FCC897,9CD24B,C864C7,D0154A,AC6462,F4B8A7,90D8F3,78312B,A47E39,A8A668,0C1262,A0EC80,E0C3F3,F46DE2,C4A366,24C44A,D058A8,D071C4,601888,4CAC0A,0026ED,002293,346987,44F436,D05BA8,D437D7,EC8A4C,208986,9CA9E4,E47723,6C8B2F,CC1AFA,F084C9,2C26C5,083FBC,6073BC,D0608C,688AF0,004A77,384608,B4B362,B075D5,08181A,002512,143EBF,94A7B7,3CDA2A,8C7967,D855A3,4C16F1,5422F8,901D27,30F31D,4C09B4,744AA4,10D0AB,90C7D8,FC2D5E,8CE117,64136C,A091C8,702E22,F41F88,78C1A7";
char *logo[]={deflogo,OnePlus,shu360,Asus,Coolpad,Dell,Haier,Hasee,Honor,HP,HTC,Huawei,Apple,Lenovo,LeEco,LG,Meitu,Meizu,OPPO,Phicomm,Samsung,Smartisan,Sony,TCL,ThinkPad,TongfangPC,VIVO,Microsoft,XiaoMi,ZTE};
uint32_t traffic_wanlan(char *ifname, uint32_t *rx, uint32_t *tx);

void wan()
{
	unsigned long long rx = 0, tx = 0, vlan_rx = 0, vlan_tx = 0;
	unsigned long long start_rx = 0, end_rx = 0;
	unsigned long long start_tx = 0, end_tx = 0;
	unsigned long long result_rx = 0, result_tx = 0;
	FILE *fp;
	char buf[2048];
	char *p, *ifname, IPV4_ADDR;
	int i, CONNECTED=0, MODE=0;
	if (swmode == 1) {
		for (i=0; i<2; i++) {
			fp = fopen("/proc/net/dev", "r");
			if (fp) {
				fgets(buf, sizeof(buf), fp);
				fgets(buf, sizeof(buf), fp);
				while (fgets(buf, sizeof(buf), fp)) {
					if ((p = strchr(buf, ':')) == NULL)
						continue;
					*p = 0;
					if ((ifname = strrchr(buf, ' ')) == NULL)
						ifname = buf;
					else
						++ifname;
					if (strcmp(ifname, "ppp0") && strcmp(ifname, "eth0") && strcmp(ifname, "vlan2"))
						continue;
					sscanf(p + 1, "%llu%*u%*u%*u%*u%*u%*u%*u%llu", &rx, &tx);
					if(strncmp(ifname, "vlan", 4)==0)
						traffic_wanlan(ifname, (uint32_t *) &rx, (uint32_t *) &tx);
					if(nvram_match("wans_dualwan", "wan none")){
						if(strcmp(ifname, "eth0")==0){
							traffic_wanlan("vlan2", (uint32_t *) &rx, (uint32_t *) &tx);
						}
					}
					if (i == 0) {
						start_rx = rx;
						start_tx = tx;
						if((start_rx > 0) && (start_tx > 0))
							break;
					} else {
						end_rx = rx;
						end_tx = tx;
						if((end_rx > 0) && (end_tx > 0))
							break;
					}
				}
				fclose(fp);
			}
			sleep(2);
		}
		result_rx = ((end_rx-start_rx)/2);
		result_tx = ((end_tx-start_tx)/2);
	}
	if(nvram_get_int("link_internet") == 2)
		CONNECTED=1;
	MODE = (swmode == 1 ? 0 : 1);
	if ((fp = fopen("/tmp/k3screenctrl/wan.sh", "w"))){
		fchmod(fileno(fp), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
		fprintf(fp, "#!/bin/sh\n");
		fprintf(fp, "echo %d\n", CONNECTED);
		fprintf(fp, "echo %s\n", nvram_get("wan0_ipaddr"));
		fprintf(fp, "echo %llu\n", result_tx);
		fprintf(fp, "echo %llu\n", result_rx);
		fprintf(fp, "echo 0\n");
		fprintf(fp, "echo %d\n", MODE);
		fclose(fp);
	}
}

int livetest(char* ip) {
	if (strstr(arpbuffer, ip))
		return 1;
	else
		return 0;
}
int find_logo(char *mac)
{
	char curmac[7];
	int i, k, j;
	int length;
	length=sizeof(logo)/sizeof(logo[0]);
	k = 0;
	while((*mac) && (k < 6)) {
		if(*mac==':') {
			mac++;
			continue;
		}
		else {
			curmac[k] = toupper(*mac);
			k++;
			mac++;
		}
	}
	for(i=0; i<length; i++)
	{
		if(strstr(logo[i],curmac))
			return i;
	}
	return 0;
}
void host()
{
	FILE *fp, *ronline, *rip;
	struct in_addr addr4;
	struct in6_addr addr6;
	char line[256];
	char *hwaddr, *ipaddr, *name, *next;
	unsigned int expires;
	int ret = 0;
	char buffer[4096],strTmp[10];
	int i, l;
	i=0;
	l=0;
	if(swmode != 1)//not router mode,go to end
		goto ONLINEEND;
	memset(buffer, 0, sizeof(buffer));
	memset(strTmp, 0, sizeof(strTmp));
	if (!(rip = fopen("/var/lib/misc/dnsmasq.leases", "r")))
	{
		//syslog(LOG_WARNING, "K3screend: dnsmasq not run,is it AP mode?\n");
		//printf("dnsmasq not run\n");
		goto ONLINEEND;
	}
	while ((next = fgets(line, sizeof(line), rip)) != NULL) {
		/* line should start from numeric value */
		if (sscanf(next, "%u ", &expires) != 1)
			continue;

		strsep(&next, " ");
		hwaddr = strsep(&next, " ") ? : "";
		ipaddr = strsep(&next, " ") ? : "";
		name = strsep(&next, " ") ? : "";

		if (inet_pton(AF_INET6, ipaddr, &addr6) != 0) {
			/* skip ipv6 leases, thay have no hwaddr, but client id */
			// hwaddr = next ? : "";
			continue;
		} else if (inet_pton(AF_INET, ipaddr, &addr4) == 0)
			continue;

		ret=livetest(ipaddr);
		if (ret)
		{
			i++;
			if (!strcmp(name, "*") || !strcmp(name, "?") || !strcmp(name, ""))
				strcpy(name, " Unknown");
			l=find_logo(hwaddr);
			strcat(buffer, "echo ");
			strcat(buffer, name);
			//strcat(buffer, "\n0\n0\n0\n");
			strcat(buffer, "\necho 0\necho 0\n");
			sprintf(strTmp, "echo %d\n", l);
			strcat(buffer, strTmp);
		}
	}
	fclose(rip);
ONLINEEND:
	if ((fp = fopen("/tmp/k3screenctrl/host.sh", "w"))){
		fchmod(fileno(fp), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
		fprintf(fp, "#!/bin/sh\n");
		fprintf(fp, "echo %d\n", i);
		if (i > 0)
			fprintf(fp, "%s", buffer);
		//fprintf(fp, "echo 0\n");
		fclose(fp);
	}
}

size_t getcontentlengthfunc(void *ptr, size_t size, size_t nmemb, void *stream) {
	int r;
	long len = 0;

	r = sscanf(ptr, "Content-Length: %ld\n", &len);
	if (r)
		*((long *) stream) = len;

	return size * nmemb;
}

size_t wirtefunc(void *ptr, size_t size, size_t nmemb, void *stream)
{
	return fwrite(ptr, size, nmemb, stream);
}

int curl_download_file(CURL *curlhandle, const char * remotepath, const char * localpath, long timeout, long tries)
{
	FILE *f;
	curl_off_t local_file_len = -1 ;
	long filesize =0 ;

	CURLcode r = CURLE_GOT_NOTHING;
	struct stat file_info;
	int use_resume = 0;
	if(stat(localpath, &file_info) == 0) 
	{
		local_file_len =  file_info.st_size;
		use_resume  = 1;
	}
	f = fopen(localpath, "ab+"); 
	if (f == NULL) {
		perror(NULL);
		return 0;
	}

	curl_easy_setopt(curlhandle, CURLOPT_URL, remotepath);
	curl_easy_setopt(curlhandle, CURLOPT_CONNECTTIMEOUT, timeout);
	curl_easy_setopt(curlhandle, CURLOPT_HEADERFUNCTION, getcontentlengthfunc);
	curl_easy_setopt(curlhandle, CURLOPT_HEADERDATA, &filesize);
	curl_easy_setopt(curlhandle, CURLOPT_RESUME_FROM_LARGE, use_resume?local_file_len:0);
	curl_easy_setopt(curlhandle, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(curlhandle, CURLOPT_SSL_VERIFYHOST, 0);
	curl_easy_setopt(curlhandle, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1); 
	curl_easy_setopt(curlhandle, CURLOPT_WRITEDATA, f);
	curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, wirtefunc);
	curl_easy_setopt(curlhandle, CURLOPT_NOPROGRESS, 1);
	curl_easy_setopt(curlhandle, CURLOPT_VERBOSE, 1);

	r = curl_easy_perform(curlhandle);
	fclose(f);

	if (r == CURLE_OK)
		return 1;
	else {
		fprintf(stderr, "%s\n", curl_easy_strerror(r));
		return 0;
	}
}

int weather()
{
	int download;
	char w[]="/tmp/weather.json";
	char url[200];
	char url1[]="https://api.seniverse.com/v3/weather/now.json";
	char url2[]="language=zh-Hans&unit=c";
	char s1[10],s2[10],s3[10],s9[64],s7[128],s8[64],s10[64];
	json_object *s4=NULL, *s5=NULL, *s6=NULL;
	FILE *fpw;
	json_object *obj = NULL;
	json_object *obj0 = NULL;
	json_object *obj1 = NULL;
	json_object *objl = NULL;
	json_object *objn = NULL;
	time_t tmpcal_ptr;
	FILE *fptime, *fpdate;
	struct tm *tmp_ptr = NULL;
	time(&tmpcal_ptr);
	tmp_ptr = localtime(&tmpcal_ptr);
	time_t timestamp = time(NULL);
	struct sysinfo info;
	sysinfo(&info);
	timer=(int)info.uptime;
	memset(s7,'\0',sizeof(s7));
	memset(s8,'\0',sizeof(s8));
	memset(s9,'\0',sizeof(s9));
	CURL *curlhandle = NULL;
	curl_global_init(CURL_GLOBAL_ALL);
	curlhandle = curl_easy_init();
	if(nvram_get_int("link_internet") != 2)
		goto wan_down;
	if(timers==0 || timers < timer || !check_if_file_exist(w)){
		timers=timer;
		timers=timer+7200;
		unlink(w);
		sprintf(s9,"%ld", timestamp);
		auth_key(s9, s8);
		sprintf(s10,"ts=%ld&ttl=300&uid=%s", timestamp, s8);
		encrypt_key(s10, s7);
		
		if(nvram_get("k3_city"))
			snprintf(url, sizeof(url), "%s?location=%s&%s&%s&sig=%s", url1, nvram_get("k3_city"), url2, s10, s7);
		else
			snprintf(url, sizeof(url), "%s?location=ip&%s&%s&sig=%s", url1, url2, s10, s7);
		//printf("\turl=%s\n", url);
		curl_download_file(curlhandle , url,w,8,3);
	}
	obj = json_object_from_file(w);
	if(json_object_object_get_ex(obj, "status_code", &obj0))//server is under DDoS attack
		goto wan_down;
	json_object_object_get_ex(obj,"results", &obj0);
	if(json_object_array_length(obj0) > 0){
		obj1=json_object_array_get_idx(obj0,0);//only one
		json_object_object_get_ex(obj1,"location", &objl);
		json_object_object_get_ex(obj1,"now", &objn);
		json_object_object_get_ex(objl,"name", &s4);
		json_object_object_get_ex(objn,"temperature", &s5);
		json_object_object_get_ex(objn,"code", &s6);
		strlcpy(s1, json_object_get_string(s4), sizeof(s1));
		strlcpy(s2, json_object_get_string(s5), sizeof(s2));
		strlcpy(s3, json_object_get_string(s6), sizeof(s3));
		json_object_put(obj);
		json_object_put(obj0);
		json_object_put(obj1);
		json_object_put(objl);
		json_object_put(objn);
		goto weather_done;
	}
wan_down:
	strlcpy(s1, "Unknow", sizeof(s1));
	strlcpy(s2, "0", sizeof(s2));
	strlcpy(s3, "0", sizeof(s3));
weather_done:
	if ((fpw = fopen("/tmp/k3screenctrl/weather.sh", "w"))){
		fchmod(fileno(fpw), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
		fprintf(fpw, "#!/bin/sh\n");
		fprintf(fpw, "echo %s\n", s1);
		fprintf(fpw, "echo %s\n", s2);
		fprintf(fpw, "echo %d-%d-%d\n", (1900+tmp_ptr->tm_year), (1+tmp_ptr->tm_mon), tmp_ptr->tm_mday);
		fprintf(fpw, "echo %02d:%02d\n", tmp_ptr->tm_hour, tmp_ptr->tm_min);
		fprintf(fpw, "echo %s\n", s3);
		fprintf(fpw, "echo %d\n", tmp_ptr->tm_wday);
		fprintf(fpw, "echo 0\n");
		fclose(fpw);
	}
	curl_easy_cleanup(curlhandle);
	curl_global_cleanup();
	return 0;
}

void basic()
{
	FILE *fpb;
	if ((fpb = fopen("/tmp/k3screenctrl/basic.sh", "w"))){
		fchmod(fileno(fpb), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
		fprintf(fpb, "#!/bin/sh\n");
		if(nvram_get("et2macaddr"))
			fprintf(fpb, "MAC_ADDR=\"%s\"\n", nvram_get("et2macaddr"));
		else
			fprintf(fpb, "MAC_ADDR=\"%s\"\n", nvram_get("et0macaddr"));
		if(swmode==1)
			fprintf(fpb, "FW_VERSION=\"%s_%s\"\n", nvram_get("buildno"), nvram_get("extendno"));
		else
			fprintf(fpb, "FW_VERSION=\"AP:%s\"\n", nvram_get("lan_ipaddr"));
		if(nvram_get("hd_version"))
			fprintf(fpb, "hd_version=%s\n", nvram_get("hd_version"));
		else
			fprintf(fpb, "hd_version=A1/A2\n");
		fprintf(fpb, "echo K3\n");
		fprintf(fpb, "echo $hd_version\n");
		fprintf(fpb, "echo $FW_VERSION\n");
		fprintf(fpb, "echo $FW_VERSION\n");
		fprintf(fpb, "echo $MAC_ADDR\n");
		fclose(fpb);
	}
}

unsigned int get_wifi_clients(int unit, int querytype)
{
	char *name, prefix[8];
	struct maclist *clientlist;
	int max_sta_count, maclist_size;
	int val, count = 0, subunit;
#ifdef RTCONFIG_WIRELESSREPEATER
	int isrepeater = 0;
#endif

	/* buffers and length */
	max_sta_count = 128;
	maclist_size = sizeof(clientlist->count) + max_sta_count * sizeof(struct ether_addr);
	clientlist = malloc(maclist_size);

	if (!clientlist)
		return 0;

	for (subunit = 0; subunit < 4; subunit++) {
#ifdef RTCONFIG_WIRELESSREPEATER
		if ((nvram_get_int("sw_mode") == SW_MODE_REPEATER) && (unit == nvram_get_int("wlc_band"))) {
			if (subunit == 0)
				continue;
			else if (subunit == 1)
				isrepeater = 1;
			else
				break;
		}
#endif

		if (subunit == 0)
			snprintf(prefix, sizeof(prefix), "wl%d_", unit);
		else
			snprintf(prefix, sizeof(prefix), "wl%d.%d_", unit, subunit);

		name = nvram_pf_safe_get(prefix, "ifname");
		if (*name == '\0') continue;

		if (subunit == 0) {
			wl_ioctl(name, WLC_GET_RADIO, &val, sizeof(val));
			if (val == 1) {
				count = -1;	// Radio is disabled
				goto exit;
			}
		}

		if ((subunit > 0) &&
#ifdef RTCONFIG_WIRELESSREPEATER
			!isrepeater &&
#endif
			!nvram_pf_get_int(prefix, "bss_enabled"))
				continue;	// Guest interface disabled

		switch (querytype) {
			case SI_WL_QUERY_AUTHE:
				strcpy((char*)clientlist, "authe_sta_list");
				if (!wl_ioctl(name, WLC_GET_VAR, clientlist, maclist_size))
					count += clientlist->count;
				break;
			case SI_WL_QUERY_AUTHO:
				strcpy((char*)clientlist, "autho_sta_list");
				if (!wl_ioctl(name, WLC_GET_VAR, clientlist, maclist_size))
					count += clientlist->count;
				break;
			case SI_WL_QUERY_ASSOC:
				clientlist->count = max_sta_count;
				if (!wl_ioctl(name, WLC_GET_ASSOCLIST, clientlist, maclist_size))
					count += clientlist->count;
				break;
		}
	}

exit:
	free(clientlist);
	return count;
}

void wifi()
{
	char *PWD_2G = NULL;
	char *PWD_5G = NULL;
#if 0
	int ENABLED_VISITOR = 0;
	char SSID_GUEST[32], PWD_GUEST[64];

	int guest_2g[3] = {0}, guest_5g[3] = {0};
	int i;
	char tmp[18];
#endif
	FILE *fp = NULL;
	if(nvram_get("screen_2G5G_pwd_en") && nvram_get_int("screen_2G5G_pwd_en")==1)
	{
		if (strcmp(nvram_get("wl0_auth_mode_x"), "open"))
		{
			PWD_2G = nvram_get("wl0_wpa_psk");
		}
		else
		{
			PWD_2G = "";
		}
		if (strcmp(nvram_get("wl1_auth_mode_x"), "open"))
		{
			PWD_5G = nvram_get("wl1_wpa_psk");
		}
		else
		{
			PWD_5G = "";
		}
	}
	else
	{
		PWD_2G = "********";
		PWD_5G = "********";
	}
#if 0		
	bzero(tmp, sizeof(tmp));
	for (i = 1; i <= 3; i++)
	{
		snprintf(tmp, sizeof(tmp), "wl0.%d_bss_enabled", i);
		guest_2g[i] = nvram_get_int(tmp);
		snprintf(tmp, sizeof(tmp), "wl1.%d_bss_enabled", i);
		guest_5g[i] = nvram_get_int(tmp);
	}

	ENABLED_VISITOR = guest_2g[1] | guest_2g[2] | guest_2g[3] | guest_5g[1] | guest_5g[2] | guest_5g[3];

	bzero(SSID_GUEST, sizeof(SSID_GUEST));
	bzero(PWD_GUEST, sizeof(PWD_GUEST));
	for (i = 1; i <= 3; i++)
	{
		if (guest_2g[i])
		{
			bzero(tmp, sizeof(tmp));
			snprintf(tmp, sizeof(tmp), "wl0.%d_ssid", i);
			strlcpy(SSID_GUEST, nvram_get(tmp), sizeof(SSID_GUEST));
			bzero(tmp, sizeof(tmp));
			snprintf(tmp, sizeof(tmp), "wl0.%d_auth_mode_x", i);
			if (strcmp(nvram_get(tmp), "open"))
			{
				bzero(tmp, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "wl0.%d_wpa_psk", i);
				strlcpy(PWD_GUEST, nvram_get(tmp), sizeof(PWD_GUEST));
			}
			break;
		}
		if (guest_5g[i])
		{
			bzero(tmp, sizeof(tmp));
			snprintf(tmp, sizeof(tmp), "wl1.%d_ssid", i);
			strlcpy(SSID_GUEST, nvram_get(tmp), sizeof(SSID_GUEST));
			bzero(tmp, sizeof(tmp));
			snprintf(tmp, sizeof(tmp), "wl1.%d_auth_mode_x", i);
			if (strcmp(nvram_get(tmp), "open"))
			{
				bzero(tmp, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "wl1.%d_wpa_psk", i);
				strlcpy(PWD_GUEST, nvram_get(tmp), sizeof(PWD_GUEST));
			}
			break;
		}
	}
	if(!nvram_get("screen_guest_pwd_en") || nvram_get_int("screen_guest_pwd_en")!=1)
		strlcpy(PWD_GUEST, "********", sizeof(PWD_GUEST));
#endif
	if ((fp = fopen("/tmp/k3screenctrl/wifi.sh", "w"))){
		fchmod(fileno(fp), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
		fprintf(fp, "#!/bin/sh\n");
		fprintf(fp, "echo %d\n", nvram_get_int("smart_connect_x"));
		fprintf(fp, "echo %s\n", nvram_get("wl0_ssid"));
		fprintf(fp, "echo \"%s\"\n", PWD_2G);
		fprintf(fp, "echo %d\n", nvram_get_int("wl0_bss_enabled"));
		fprintf(fp, "echo %d\n", get_wifi_clients(0,SI_WL_QUERY_ASSOC));
		fprintf(fp, "echo %s\n", nvram_get("wl1_ssid"));
		fprintf(fp, "echo \"%s\"\n", PWD_5G);
		fprintf(fp, "echo %d\n", nvram_get_int("wl1_bss_enabled"));
		fprintf(fp, "echo %d\n", get_wifi_clients(1,SI_WL_QUERY_ASSOC));
#if 0
		fprintf(fp, "echo %s\n", SSID_GUEST);
		fprintf(fp, "echo %s\n", PWD_GUEST);
		fprintf(fp, "echo %d\n", ENABLED_VISITOR);
#else
		fprintf(fp, "echo SSID_GUEST\n");
		fprintf(fp, "echo \"********\"\n");
		fprintf(fp, "echo 0\n");
#endif
		fprintf(fp, "echo 0\n");
		fclose(fp);
	}
}
void port()
{
	FILE *fpp;
	int ports[4];
	int status[4];
	int i, lret=0, mask;
	char *usb =nvram_get("usb_path1_fs_path0");
	ports[0]=3; ports[1]=1; ports[2]=0; ports[3]=2;

	for (i=0; i<4; i++)
	{
		mask = 0;
		mask |= 0x0001<<ports[i];
		if (get_phy_status(mask)==0)
			status[i]=0;
		else
			status[i]=1;
	}
	if(usb!=NULL && strlen(usb)>1)
		lret=1;
	if ((fpp = fopen("/tmp/k3screenctrl/port.sh", "w"))){
		fchmod(fileno(fpp), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
		fprintf(fpp, "#!/bin/sh\n");
		fprintf(fpp, "echo %d\n",status[1]);
		fprintf(fpp, "echo %d\n",status[2]);
		fprintf(fpp, "echo %d\n",status[3]);
		fprintf(fpp, "echo %d\n",status[0]);
		fprintf(fpp, "echo %d\n",lret);
		fclose(fpp);
	}
}
static void check_K3screend_exit(int sig)
{
	syslog(LOG_ERR, "K3screend: exit\n");
	exit(0);
}

void check_K3screend(int sig)
{
	FILE * pip;
	basic();
	wifi();
	wan();
	port();
	host();
	weather();
	memset(arpbuffer, 0, sizeof(arpbuffer));
	if ((pip=popen("cat /proc/net/arp", "r")) == NULL ){
		syslog(LOG_ERR, "K3screend: can't open /proc/net/arp\n");
		return;//bug?
	}
	fread(arpbuffer, 1, sizeof(arpbuffer), pip);
	pclose(pip);
	alarm(10);
}

static void
alarmtimer(unsigned long sec, unsigned long usec)
{
	itv.it_value.tv_sec = sec;
	itv.it_value.tv_usec = usec;
	itv.it_interval.tv_sec = 1;
	itv.it_interval.tv_usec = usec;
	setitimer(ITIMER_REAL, &itv, NULL);
}

int main(int argc, char * argv[])
{
	sigset_t sigs_to_catch;
	swmode=nvram_get_int("sw_mode");//1=router,2=ap,3=rp or aimesh,4=mb
	time_t ts = time(NULL);
	if (ts < 1591016709)//waiting for time sync
		check_K3screend_exit(0);
	sigemptyset(&sigs_to_catch);
	sigaddset(&sigs_to_catch, SIGTERM);
	sigaddset(&sigs_to_catch, SIGALRM);
	sigprocmask(SIG_UNBLOCK, &sigs_to_catch, NULL);
	signal(SIGTERM, check_K3screend_exit);
	signal(SIGALRM, check_K3screend);
	syslog(LOG_ERR, "K3screend: start\n");
	alarm(1);
	while(1)
		pause();
	return 0;
}

