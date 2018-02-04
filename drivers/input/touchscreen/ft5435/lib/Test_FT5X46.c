/************************************************************************
* Copyright (C) 2012-2015, Focaltech Systems (R)��All Rights Reserved.
*
* File Name: Test_FT5X46.c
*
* Author: Software Development Team, AE
*
* Created: 2015-07-14
*
* Abstract: test item for FT5X46\FT5X46i\FT5526\FT3X17\FT5436\FT3X27\FT5526i\FT5416\FT5426\FT5435
*
************************************************************************/

/*******************************************************************************
* Included header files
*******************************************************************************/
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/slab.h>

#include "Global.h"
#include "DetailThreshold.h"
#include "Test_FT5X46.h"
#include "Config_FT5X46.h"


/*******************************************************************************
* Private constant and macro definitions using #define
*******************************************************************************/
#define IC_TEST_VERSION  "Test version: V1.1.0--2015-10-22, (sync version of FT_MultipleTest: V2.7.0.3--2015-07-13)"


#define DEVIDE_MODE_ADDR	0x00
#define REG_LINE_NUM	0x01
#define REG_TX_NUM	0x02
#define REG_RX_NUM	0x03
#define REG_PATTERN_5422        0x53
#define REG_MAPPING_SWITCH      0x54
#define REG_TX_NOMAPPING_NUM        0x55
#define REG_RX_NOMAPPING_NUM      0x56
#define REG_NORMALIZE_TYPE      0x16
#define REG_ScCbBuf0	0x4E
#define REG_ScWorkMode	0x44
#define REG_ScCbAddrR	0x45
#define REG_RawBuf0 0x36
#define REG_WATER_CHANNEL_SELECT 0x09

/*******************************************************************************
* Private enumerations, structures and unions using typedef
*******************************************************************************/
enum WaterproofType
{
	WT_NeedProofOnTest,
	WT_NeedProofOffTest,
	WT_NeedTxOnVal,
	WT_NeedRxOnVal,
	WT_NeedTxOffVal,
	WT_NeedRxOffVal,
};
/*******************************************************************************
* Static variables
*******************************************************************************/

static int m_RawData[TX_NUM_MAX][RX_NUM_MAX] = {{0, 0} } ;
static int m_iTempRawData[TX_NUM_MAX * RX_NUM_MAX] = {0};
static unsigned char m_ucTempData[TX_NUM_MAX * RX_NUM_MAX*2] = {0};
static bool m_bV3TP


static char g_pStoreAllData[1024*80] = {0};
static char *g_pTmpBuff
static char *g_pStoreMsgArea
static int g_lenStoreMsgArea
static char *g_pMsgAreaLine2
static int g_lenMsgAreaLine2
static char *g_pStoreDataArea
static int g_lenStoreDataArea
static unsigned char m_ucTestItemCode
static int m_iStartLine
static int m_iTestDataCount

/*******************************************************************************
* Global variable or extern global variabls/functions
*******************************************************************************/


/*******************************************************************************
* Static function prototypes
*******************************************************************************/

static int StartScan(void);
static unsigned char ReadRawData(unsigned char Freq, unsigned char LineNum, int ByteNum, int *pRevBuffer);
static unsigned char GetPanelRows(unsigned char *pPanelRows);
static unsigned char GetPanelCols(unsigned char *pPanelCols);
static unsigned char GetTxSC_CB(unsigned char index, unsigned char *pcbValue);

static unsigned char GetRawData(void);
static unsigned char GetChannelNum(void);

static int InitTest(void);
static void FinishTest(void);
static void Save_Test_Data(int iData[TX_NUM_MAX][RX_NUM_MAX], int iArrayIndex, unsigned char Row, unsigned char Col, unsigned char ItemCount);
static void InitStoreParamOfTestData(void);
static void MergeAllTestData(void);

static int AllocateMemory(void);

static void ShowRawData(void);
static boolean GetTestCondition(int iTestType, unsigned char ucChannelValue);

static unsigned char GetChannelNumNoMapping(void);
static unsigned char SwitchToNoMapping(void);


/************************************************************************
* Name: FT5X46_StartTest
* Brief:  Test entry. Determine which test item to test
* Input: none
* Output: none
* Return: Test Result, PASS or FAIL
***********************************************************************/
boolean FT5X46_StartTest()
{
	bool bTestResult = true;
	bool bTempResult = 1;
	unsigned char ReCode = 0;
	unsigned char ucDevice = 0;
	int iItemCount = 0;


	if (InitTest() < 0) {
		bTestResult = false;
		return bTestResult;
	}


	if (0 == g_TestItemNum)
		bTestResult = false;

	for (iItemCount = 0; iItemCount < g_TestItemNum; iItemCount++) {
		m_ucTestItemCode = g_stTestItem[ucDevice][iItemCount].ItemCode;


		if (Code_FT5X22_ENTER_FACTORY_MODE == g_stTestItem[ucDevice][iItemCount].ItemCode
			) {
			ReCode = FT5X46_TestItem_EnterFactoryMode();
			if (ERROR_CODE_OK != ReCode || (!bTempResult)) {
				bTestResult = false;
				g_stTestItem[ucDevice][iItemCount].TestResult = RESULT_NG;
				break;
			} else
				g_stTestItem[ucDevice][iItemCount].TestResult = RESULT_PASS;
		}

		if (Code_FT5X22_RAWDATA_TEST == g_stTestItem[ucDevice][iItemCount].ItemCode
			) {
			ReCode = FT5X46_TestItem_RawDataTest(&bTempResult);
			if (ERROR_CODE_OK != ReCode || (!bTempResult)) {
				bTestResult = false;
				g_stTestItem[ucDevice][iItemCount].TestResult = RESULT_NG;
			} else
				g_stTestItem[ucDevice][iItemCount].TestResult = RESULT_PASS;
		}



		if (Code_FT5X22_SCAP_CB_TEST == g_stTestItem[ucDevice][iItemCount].ItemCode
			) {
			ReCode = FT5X46_TestItem_SCapCbTest(&bTempResult);
			if (ERROR_CODE_OK != ReCode || (!bTempResult)) {
				bTestResult = false;
				g_stTestItem[ucDevice][iItemCount].TestResult = RESULT_NG;
			} else
				g_stTestItem[ucDevice][iItemCount].TestResult = RESULT_PASS;
		}


		if (Code_FT5X22_SCAP_RAWDATA_TEST == g_stTestItem[ucDevice][iItemCount].ItemCode
			) {
			ReCode = FT5X46_TestItem_SCapRawDataTest(&bTempResult);
			if (ERROR_CODE_OK != ReCode || (!bTempResult)) {
				bTestResult = false;
				g_stTestItem[ucDevice][iItemCount].TestResult = RESULT_NG;
			} else
				g_stTestItem[ucDevice][iItemCount].TestResult = RESULT_PASS;
		}


	}


	FinishTest();


	return bTestResult;
}
/************************************************************************
* Name: InitTest
* Brief:  Init all param before test
* Input: none
* Output: none
* Return: none
***********************************************************************/
static int InitTest(void) {
	if (AllocateMemory() < 0) {
		return -EPERM;
	}
	InitStoreParamOfTestData();
	printk("[focal] %s \n", IC_TEST_VERSION);
	return 1;
}
/************************************************************************
* Name: FinishTest
* Brief:  Init all param before test
* Input: none
* Output: none
* Return: none
***********************************************************************/
static void FinishTest(void)
{
	MergeAllTestData();

}
/************************************************************************
* Name: FT5X46_get_test_data
* Brief:  get data of test result
* Input: none
* Output: pTestData, the returned buff
* Return: the length of test data. if length > 0, got data;else ERR.
***********************************************************************/
int FT5X46_get_test_data(char *pTestData) {
	if (NULL == pTestData) {
		printk("[focal] %s pTestData == NULL \n", __func__);
		return -EPERM;
	}
	memcpy(pTestData, g_pStoreAllData, (g_lenStoreMsgArea+g_lenStoreDataArea));
	return (g_lenStoreMsgArea+g_lenStoreDataArea);
}

/************************************************************************
* Name: FT5X46_TestItem_EnterFactoryMode
* Brief:  Check whether TP can enter Factory Mode, and do some thing
* Input: none
* Output: none
* Return: Comm Code. Code = 0x00 is OK, else fail.
***********************************************************************/
unsigned char FT5X46_TestItem_EnterFactoryMode(void)
{
	unsigned char ReCode = ERROR_CODE_INVALID_PARAM;
	int iRedo = 5;
	int i ;
	unsigned char chPattern = 0;

	SysDelay(150);
	for (i = 1; i <= iRedo; i++) {
		ReCode = EnterFactory();
		if (ERROR_CODE_OK != ReCode) {
			printk("Failed to Enter factory mode...\n");
			if (i < iRedo) {
				SysDelay(50);
				continue;
			}
		} else {
			printk("Succeed to Enter factory mode...\n");
			break;
		}

	}
	SysDelay(300);


	if (ReCode != ERROR_CODE_OK) {
		return ReCode;
	}

	ReCode = GetChannelNum();

	ReCode = ReadReg(REG_PATTERN_5422, &chPattern);
	if (chPattern == 1) {
		m_bV3TP = true;
	} else {
		m_bV3TP = false;
	}

	return ReCode;
}
/************************************************************************
* Name: FT5X46_TestItem_RawDataTest
* Brief:  TestItem: RawDataTest. Check if MCAP RawData is within the range.
* Input: none
* Output: bTestResult, PASS or FAIL
* Return: Comm Code. Code = 0x00 is OK, else fail.
***********************************************************************/
unsigned char FT5X46_TestItem_RawDataTest(bool *bTestResult)
{
	unsigned char ReCode = 0;
	bool btmpresult = true;
	int RawDataMin;
	int RawDataMax;
	unsigned char ucFre;
	unsigned char strSwitch = 0;
	unsigned char OriginValue = 0xff;
	int index = 0;
	int iRow, iCol;
	int iValue = 0;


	printk("\n\n==============================Test Item: -------- Raw Data  Test \n\n");
	ReCode = EnterFactory();
	if (ReCode != ERROR_CODE_OK) {
		printk("\n\n// Failed to Enter factory Mode. Error Code: %d", ReCode);
		goto TEST_ERR;
	}


	if (m_bV3TP) {
		ReCode = ReadReg(REG_MAPPING_SWITCH, &strSwitch);
		if (ReCode != ERROR_CODE_OK) {
			printk("\n Read REG_MAPPING_SWITCH error. Error Code: %d\n", ReCode);
			goto TEST_ERR;
		}

		if (strSwitch != 0) {
			ReCode = WriteReg(REG_MAPPING_SWITCH, 0);
			if (ReCode != ERROR_CODE_OK) {
				printk("\n Write REG_MAPPING_SWITCH error. Error Code: %d\n", ReCode);
				goto TEST_ERR;
			}
		}
	}

	ReCode = ReadReg(REG_NORMALIZE_TYPE, &OriginValue);
	if (ReCode != ERROR_CODE_OK) {
		printk("\n Read  REG_NORMALIZE_TYPE error. Error Code: %d\n", ReCode);
		goto TEST_ERR;
	}

	if (g_ScreenSetParam.isNormalize == Auto_Normalize) {
		if (OriginValue != 1) {
			ReCode = WriteReg(REG_NORMALIZE_TYPE, 0x01);
			if (ReCode != ERROR_CODE_OK) {
				printk("\n write  REG_NORMALIZE_TYPE error. Error Code: %d\n", ReCode);
				goto TEST_ERR;
			}
		}

		printk("\n=========Set Frequecy High\n");
		ReCode = WriteReg(0x0A, 0x81);
		if (ReCode != ERROR_CODE_OK) {
			printk("\n Set Frequecy High error. Error Code: %d\n", ReCode);
			goto TEST_ERR;
		}

		printk("\n=========FIR State: ON\n");
		ReCode = WriteReg(0xFB, 1);
		if (ReCode != ERROR_CODE_OK) {
			printk("\n FIR State: ON error. Error Code: %d\n", ReCode);
			goto TEST_ERR;
		}

		for (index = 0; index < 3; ++index) {
			ReCode = GetRawData();
		}

		if (ReCode != ERROR_CODE_OK) {
			printk("\nGet Rawdata failed, Error Code: 0x%x",  ReCode);
			goto TEST_ERR;
		}

		ShowRawData();


		for (iRow = 0; iRow < g_ScreenSetParam.iTxNum; iRow++) {
			for (iCol = 0; iCol < g_ScreenSetParam.iRxNum; iCol++) {
				if (g_stCfg_MCap_DetailThreshold.InvalidNode[iRow][iCol] == 0)
					continue;
				RawDataMin = g_stCfg_MCap_DetailThreshold.RawDataTest_High_Min[iRow][iCol];
				RawDataMax = g_stCfg_MCap_DetailThreshold.RawDataTest_High_Max[iRow][iCol];
				iValue = m_RawData[iRow][iCol];
				if (iValue < RawDataMin || iValue > RawDataMax) {
					btmpresult = false;
					printk("rawdata test failure. Node=(%d,  %d), Get_value=%d,  Set_Range=(%d, %d) \n", \
						iRow+1, iCol+1, iValue, RawDataMin, RawDataMax);
				}
			}
		}


		Save_Test_Data(m_RawData, 0, g_ScreenSetParam.iTxNum, g_ScreenSetParam.iRxNum, 1);
	} else {
		if (OriginValue != 0) {
			ReCode = WriteReg(REG_NORMALIZE_TYPE, 0x00);
			if (ReCode != ERROR_CODE_OK) {
				printk("\n write REG_NORMALIZE_TYPE error. Error Code: %d\n", ReCode);
				goto TEST_ERR;
			}
		}

		ReCode =  ReadReg(0x0A, &ucFre);
		if (ReCode != ERROR_CODE_OK) {
			printk("\n Read frequency error. Error Code: %d\n", ReCode);
			goto TEST_ERR;
		}


		if (g_stCfg_FT5X22_BasicThreshold.RawDataTest_SetLowFreq) {
			printk("\n=========Set Frequecy Low\n");
			ReCode = WriteReg(0x0A, 0x80);
			if (ReCode != ERROR_CODE_OK) {
				printk("\n write frequency error. Error Code: %d\n", ReCode);
				goto TEST_ERR;
			}

			printk("\n=========FIR State: OFF\n");
			ReCode = WriteReg(0xFB, 0);
			if (ReCode != ERROR_CODE_OK) {
				printk("\n FIR State: OFF error. Error Code: %d\n", ReCode);
				goto TEST_ERR;
			}
			SysDelay(100);
			for (index = 0; index < 3; ++index) {
				ReCode = GetRawData();
			}

			if (ReCode != ERROR_CODE_OK) {
				printk("\nGet Rawdata failed, Error Code: 0x%x",  ReCode);
				goto TEST_ERR;
			}
			ShowRawData();


			for (iRow = 0; iRow < g_ScreenSetParam.iTxNum; iRow++) {

				for (iCol = 0; iCol < g_ScreenSetParam.iRxNum; iCol++) {
					if (g_stCfg_MCap_DetailThreshold.InvalidNode[iRow][iCol] == 0)
						continue;
					RawDataMin = g_stCfg_MCap_DetailThreshold.RawDataTest_High_Min[iRow][iCol];
					RawDataMax = g_stCfg_MCap_DetailThreshold.RawDataTest_High_Max[iRow][iCol];
					iValue = m_RawData[iRow][iCol];
					if (iValue < RawDataMin || iValue > RawDataMax) {
						btmpresult = false;
						printk("rawdata test failure. Node=(%d,  %d), Get_value=%d,  Set_Range=(%d, %d) \n", \
							iRow+1, iCol+1, iValue, RawDataMin, RawDataMax);
					}
				}
			}


			Save_Test_Data(m_RawData, 0, g_ScreenSetParam.iTxNum, g_ScreenSetParam.iRxNum, 1);
		}


		if (g_stCfg_FT5X22_BasicThreshold.RawDataTest_SetHighFreq) {

			printk("\n=========Set Frequecy High\n");
			ReCode = WriteReg(0x0A, 0x81);
			if (ReCode != ERROR_CODE_OK) {
				printk("\n Set Frequecy High error. Error Code: %d\n", ReCode);
				goto TEST_ERR;
			}

			printk("\n=========FIR State: OFF\n");
			ReCode = WriteReg(0xFB, 0);
			if (ReCode != ERROR_CODE_OK) {
				printk("\n FIR State: OFF error. Error Code: %d\n", ReCode);
				goto TEST_ERR;
			}
			SysDelay(100);
			for (index = 0; index < 3; ++index) {
				ReCode = GetRawData();
			}

			if (ReCode != ERROR_CODE_OK) {
				printk("\nGet Rawdata failed, Error Code: 0x%x",  ReCode);
				if (ReCode != ERROR_CODE_OK)goto TEST_ERR;
			}
			ShowRawData();


			for (iRow = 0; iRow < g_ScreenSetParam.iTxNum; iRow++) {

				for (iCol = 0; iCol < g_ScreenSetParam.iRxNum; iCol++) {
					if (g_stCfg_MCap_DetailThreshold.InvalidNode[iRow][iCol] == 0)
						continue;
					RawDataMin = g_stCfg_MCap_DetailThreshold.RawDataTest_High_Min[iRow][iCol];
					RawDataMax = g_stCfg_MCap_DetailThreshold.RawDataTest_High_Max[iRow][iCol];
					iValue = m_RawData[iRow][iCol];
					if (iValue < RawDataMin || iValue > RawDataMax) {
						btmpresult = false;
						printk("rawdata test failure. Node=(%d,  %d), Get_value=%d,  Set_Range=(%d, %d) \n", \
							iRow+1, iCol+1, iValue, RawDataMin, RawDataMax);
					}
				}
			}


			Save_Test_Data(m_RawData, 0, g_ScreenSetParam.iTxNum, g_ScreenSetParam.iRxNum, 2);
		}

	}



	ReCode = WriteReg(REG_NORMALIZE_TYPE, OriginValue);
	if (ReCode != ERROR_CODE_OK) {
		printk("\n Write REG_NORMALIZE_TYPE error. Error Code: %d\n", ReCode);
		goto TEST_ERR;
	}

	if (m_bV3TP) {
		ReCode = WriteReg(REG_MAPPING_SWITCH, strSwitch);
		if (ReCode != ERROR_CODE_OK) {
			printk("\n Write REG_MAPPING_SWITCH error. Error Code: %d\n", ReCode);
			goto TEST_ERR;
		}
	}


	if (btmpresult) {
		*bTestResult = true;
		printk("\n\n//RawData Test is OK!\n");
	} else {
		*bTestResult = false;
		printk("\n\n//RawData Test is NG!\n");
	}
	return ReCode;

TEST_ERR:

	*bTestResult = false;
	printk("\n\n//RawData Test is NG!\n");
	return ReCode;

}
/************************************************************************
* Name: FT5X46_TestItem_SCapRawDataTest
* Brief:  TestItem: SCapRawDataTest. Check if SCAP RawData is within the range.
* Input: none
* Output: bTestResult, PASS or FAIL
* Return: Comm Code. Code = 0x00 is OK, else fail.
***********************************************************************/
unsigned char FT5X46_TestItem_SCapRawDataTest(bool *bTestResult)
{
	int i = 0;
	int RawDataMin = 0;
	int RawDataMax = 0;
	int Value = 0;
	boolean bFlag = true;
	unsigned char ReCode = 0;
	boolean btmpresult = true;
	int iMax = 0;
	int iMin = 0;
	int iAvg = 0;
	int ByteNum = 0;
	unsigned char wc_value = 0;
	unsigned char ucValue = 0;
	int iCount = 0;
	int ibiggerValue = 0;

	printk("\n\n==============================Test Item: -------- Scap RawData Test \n\n");


	ReCode = EnterFactory();
	if (ReCode != ERROR_CODE_OK) {
		printk("\n\n// Failed to Enter factory Mode. Error Code: %d", ReCode);
		goto TEST_ERR;
	}


	ReCode = ReadReg(REG_WATER_CHANNEL_SELECT, &wc_value);
	if (ReCode != ERROR_CODE_OK) {
		printk("\n\n// Failed to read REG_WATER_CHANNEL_SELECT. Error Code: %d", ReCode);
		goto TEST_ERR;
	}


	ReCode = SwitchToNoMapping();
	if (ReCode != ERROR_CODE_OK) {
		printk("\n\n// Failed to SwitchToNoMapping. Error Code: %d", ReCode);
		goto TEST_ERR;
	}


	ReCode = StartScan();
	if (ReCode != ERROR_CODE_OK) {
		printk("Failed to Scan SCap RawData! \n");
		goto TEST_ERR;
	}
	for (i = 0; i < 3; i++) {
		memset(m_iTempRawData, 0, sizeof(m_iTempRawData));

		ByteNum = (g_ScreenSetParam.iTxNum + g_ScreenSetParam.iRxNum)*2;
		ReCode = ReadRawData(0, 0xAC, ByteNum, m_iTempRawData);
		if (ReCode != ERROR_CODE_OK) {
			printk("Failed to ReadRawData water! \n");
			goto TEST_ERR;
		}

		memcpy(m_RawData[0+g_ScreenSetParam.iTxNum], m_iTempRawData, sizeof(int)*g_ScreenSetParam.iRxNum);
		memcpy(m_RawData[1+g_ScreenSetParam.iTxNum], m_iTempRawData + g_ScreenSetParam.iRxNum, sizeof(int)*g_ScreenSetParam.iTxNum);
		ByteNum = (g_ScreenSetParam.iTxNum + g_ScreenSetParam.iRxNum)*2;
		ReCode = ReadRawData(0, 0xAB, ByteNum, m_iTempRawData);
		if (ReCode != ERROR_CODE_OK) {
			printk("Failed to ReadRawData no water! \n");
			goto TEST_ERR;
		}
		memcpy(m_RawData[2+g_ScreenSetParam.iTxNum], m_iTempRawData, sizeof(int)*g_ScreenSetParam.iRxNum);
		memcpy(m_RawData[3+g_ScreenSetParam.iTxNum], m_iTempRawData + g_ScreenSetParam.iRxNum, sizeof(int)*g_ScreenSetParam.iTxNum);
	}





	bFlag = GetTestCondition(WT_NeedProofOnTest, wc_value);
	if (g_stCfg_FT5X22_BasicThreshold.SCapRawDataTest_SetWaterproof_ON && bFlag) {
		iCount = 0;
		RawDataMin = g_stCfg_FT5X22_BasicThreshold.SCapRawDataTest_ON_Min;
		RawDataMax = g_stCfg_FT5X22_BasicThreshold.SCapRawDataTest_ON_Max;
		iMax = -m_RawData[0+g_ScreenSetParam.iTxNum][0];
		iMin = 2 * m_RawData[0+g_ScreenSetParam.iTxNum][0];
		iAvg = 0;
		Value = 0;


		bFlag = GetTestCondition(WT_NeedRxOnVal, wc_value);
		if (bFlag)
			printk("Judge Rx in Waterproof-ON:\n");
		for (i = 0; bFlag && i < g_ScreenSetParam.iRxNum; i++) {
			if (g_stCfg_MCap_DetailThreshold.InvalidNode_SC[0][i] == 0)
				continue;
			RawDataMin = g_stCfg_MCap_DetailThreshold.SCapRawDataTest_ON_Min[0][i];
			RawDataMax = g_stCfg_MCap_DetailThreshold.SCapRawDataTest_ON_Max[0][i];
			Value = m_RawData[0+g_ScreenSetParam.iTxNum][i];
			iAvg += Value;
			if (iMax < Value)
				iMax = Value;
			if (iMin > Value)
				iMin = Value;
			if (Value > RawDataMax || Value < RawDataMin) {
				btmpresult = false;
				printk("Failed. Num = %d, Value = %d, range = (%d, %d):\n", i+1, Value, RawDataMin, RawDataMax);
			}
			iCount++;
		}


		bFlag = GetTestCondition(WT_NeedTxOnVal, wc_value);
		if (bFlag)
			printk("Judge Tx in Waterproof-ON:\n");
		for (i = 0; bFlag && i < g_ScreenSetParam.iTxNum; i++) {
			if (g_stCfg_MCap_DetailThreshold.InvalidNode_SC[1][i] == 0)
				continue;
			RawDataMin = g_stCfg_MCap_DetailThreshold.SCapRawDataTest_ON_Min[1][i];
			RawDataMax = g_stCfg_MCap_DetailThreshold.SCapRawDataTest_ON_Max[1][i];
			Value = m_RawData[1+g_ScreenSetParam.iTxNum][i];
			iAvg += Value;
			if (iMax < Value)
				iMax = Value;
			if (iMin > Value)
				iMin = Value;
			if (Value > RawDataMax || Value < RawDataMin) {
				btmpresult = false;
				printk("Failed. Num = %d, Value = %d, range = (%d, %d):\n", i+1, Value, RawDataMin, RawDataMax);
			}
			iCount++;
		}
		if (0 == iCount) {
			iAvg = 0;
			iMax = 0;
			iMin = 0;
		} else
			iAvg = iAvg/iCount;

		printk("SCap RawData in Waterproof-ON, Max : %d, Min: %d, Deviation: %d, Average: %d\n", iMax, iMin, iMax - iMin, iAvg);

		ibiggerValue = g_ScreenSetParam.iTxNum > g_ScreenSetParam.iRxNum?g_ScreenSetParam.iTxNum:g_ScreenSetParam.iRxNum;
		Save_Test_Data(m_RawData, g_ScreenSetParam.iTxNum+0, 2, ibiggerValue, 1);
	}


	bFlag = GetTestCondition(WT_NeedProofOffTest, wc_value);
	if (g_stCfg_FT5X22_BasicThreshold.SCapRawDataTest_SetWaterproof_OFF && bFlag) {
		iCount = 0;
		RawDataMin = g_stCfg_FT5X22_BasicThreshold.SCapRawDataTest_OFF_Min;
		RawDataMax = g_stCfg_FT5X22_BasicThreshold.SCapRawDataTest_OFF_Max;
		iMax = -m_RawData[2+g_ScreenSetParam.iTxNum][0];
		iMin = 2 * m_RawData[2+g_ScreenSetParam.iTxNum][0];
		iAvg = 0;
		Value = 0;

		bFlag = GetTestCondition(WT_NeedRxOffVal, wc_value);
		if (bFlag)
			printk("Judge Rx in Waterproof-OFF:\n");
		for (i = 0; bFlag && i < g_ScreenSetParam.iRxNum; i++) {
			if (g_stCfg_MCap_DetailThreshold.InvalidNode_SC[0][i] == 0)
				continue;
			RawDataMin = g_stCfg_MCap_DetailThreshold.SCapRawDataTest_OFF_Min[0][i];
			RawDataMax = g_stCfg_MCap_DetailThreshold.SCapRawDataTest_OFF_Max[0][i];
			Value = m_RawData[2+g_ScreenSetParam.iTxNum][i];
			iAvg += Value;



			if (iMax < Value)
				iMax = Value;
			if (iMin > Value)
				iMin = Value;
			if (Value > RawDataMax || Value < RawDataMin) {
				btmpresult = false;
				printk("Failed. Num = %d, Value = %d, range = (%d, %d):\n", i+1, Value, RawDataMin, RawDataMax);
			}
			iCount++;
		}

		bFlag = GetTestCondition(WT_NeedTxOffVal, wc_value);
		if (bFlag)
			printk("Judge Tx in Waterproof-OFF:\n");
		for (i = 0; bFlag && i < g_ScreenSetParam.iTxNum; i++) {
			if (g_stCfg_MCap_DetailThreshold.InvalidNode_SC[1][i] == 0)
				continue;

			Value = m_RawData[3+g_ScreenSetParam.iTxNum][i];
			RawDataMin = g_stCfg_MCap_DetailThreshold.SCapRawDataTest_OFF_Min[1][i];
			RawDataMax = g_stCfg_MCap_DetailThreshold.SCapRawDataTest_OFF_Max[1][i];

			iAvg += Value;
			if (iMax < Value)
				iMax = Value;
			if (iMin > Value)
				iMin = Value;
			if (Value > RawDataMax || Value < RawDataMin) {
				btmpresult = false;
				printk("Failed. Num = %d, Value = %d, range = (%d, %d):\n", i+1, Value, RawDataMin, RawDataMax);
			}
			iCount++;
		}
		if (0 == iCount) {
			iAvg = 0;
			iMax = 0;
			iMin = 0;
		} else
			iAvg = iAvg/iCount;

		printk("SCap RawData in Waterproof-OFF, Max : %d, Min: %d, Deviation: %d, Average: %d\n", iMax, iMin, iMax - iMin, iAvg);

		ibiggerValue = g_ScreenSetParam.iTxNum > g_ScreenSetParam.iRxNum?g_ScreenSetParam.iTxNum:g_ScreenSetParam.iRxNum;
		Save_Test_Data(m_RawData, g_ScreenSetParam.iTxNum+2, 2, ibiggerValue, 2);
	}

	if (m_bV3TP) {
		ReCode = ReadReg(REG_MAPPING_SWITCH, &ucValue);
		if (ReCode != ERROR_CODE_OK) {
			printk("\n Read REG_MAPPING_SWITCH error. Error Code: %d\n", ReCode);
			goto TEST_ERR;
		}

		if (0 != ucValue) {
			ReCode = WriteReg(REG_MAPPING_SWITCH, 0);
			SysDelay(10);
			if (ReCode != ERROR_CODE_OK) {
				printk("Failed to switch mapping type!\n ");
				btmpresult = false;
			}
		}

		ReCode = GetChannelNum();
		if (ReCode != ERROR_CODE_OK) {
			printk("\n GetChannelNum error. Error Code: %d\n", ReCode);
			goto TEST_ERR;
		}
	}


	if (btmpresult) {
		*bTestResult = true;
		printk("\n\n//SCap RawData Test is OK!\n");
	} else {
		*bTestResult = false;
		printk("\n\n//SCap RawData Test is NG!\n");
	}
	return ReCode;

TEST_ERR:
	*bTestResult = false;
	printk("\n\n//SCap RawData Test is NG!\n");
	return ReCode;
}

/************************************************************************
* Name: FT5X46_TestItem_SCapCbTest
* Brief:  TestItem: SCapCbTest. Check if SCAP Cb is within the range.
* Input: none
* Output: bTestResult, PASS or FAIL
* Return: Comm Code. Code = 0x00 is OK, else fail.
***********************************************************************/ {
	int i, /* j, iOutNum, */index, Value, CBMin, CBMax;
	boolean bFlag = true;
	unsigned char ReCode;
	boolean btmpresult = true;
	int iMax, iMin, iAvg;
	unsigned char wc_value = 0;
	unsigned char ucValue = 0;
	int iCount = 0;
	int ibiggerValue = 0;

	printk("\n\n==============================Test Item: -----  Scap CB Test \n\n");


	ReCode = EnterFactory();
	if (ReCode != ERROR_CODE_OK) {
		printk("\n\n// Failed to Enter factory Mode. Error Code: %d", ReCode);
		goto TEST_ERR;
	}


	ReCode = ReadReg(REG_WATER_CHANNEL_SELECT, &wc_value);
	if (ReCode != ERROR_CODE_OK) {
		printk("\n Read REG_WATER_CHANNEL_SELECT error. Error Code: %d\n", ReCode);
		goto TEST_ERR;
	}


	bFlag = SwitchToNoMapping();
	if (bFlag) {
		printk("Failed to SwitchToNoMapping! ReCode = %d. \n", ReCode);
		goto TEST_ERR;
	}


	ReCode = StartScan();
	if (ReCode != ERROR_CODE_OK) {
		printk("Failed to Scan SCap RawData!ReCode = %d. \n", ReCode);
		goto TEST_ERR;
	}


	for (i = 0; i < 3; i++) {
		memset(m_RawData, 0, sizeof(m_RawData));
		memset(m_ucTempData, 0, sizeof(m_ucTempData));

		ReCode = WriteReg(REG_ScWorkMode, 1);
		if (ReCode != ERROR_CODE_OK) {
			printk("Get REG_ScWorkMode Failed!\n");
			goto TEST_ERR;
		}

		ReCode = StartScan();
		if (ReCode != ERROR_CODE_OK) {
			printk("StartScan Failed!\n");
			goto TEST_ERR;
		}

		ReCode = WriteReg(REG_ScCbAddrR, 0);
		if (ReCode != ERROR_CODE_OK) {
			printk("Write REG_ScCbAddrR Failed!\n");
			goto TEST_ERR;
		}

		ReCode = GetTxSC_CB(g_ScreenSetParam.iTxNum + g_ScreenSetParam.iRxNum + 128, m_ucTempData);
		if (ReCode != ERROR_CODE_OK) {
			printk("GetTxSC_CB Failed!\n");
			goto TEST_ERR;
		}

		for (index = 0; index < g_ScreenSetParam.iRxNum; ++index) {
			m_RawData[0 + g_ScreenSetParam.iTxNum][index] = m_ucTempData[index];
		}
		for (index = 0; index < g_ScreenSetParam.iTxNum; ++index) {
			m_RawData[1 + g_ScreenSetParam.iTxNum][index] = m_ucTempData[index + g_ScreenSetParam.iRxNum];
		}

		ReCode = WriteReg(REG_ScWorkMode, 0);
		if (ReCode != ERROR_CODE_OK) {
			printk("Get REG_ScWorkMode Failed!\n");
			goto TEST_ERR;
		}

		ReCode = StartScan();
		if (ReCode != ERROR_CODE_OK) {
			printk("StartScan Failed!\n");
			goto TEST_ERR;
		}

		ReCode = WriteReg(REG_ScCbAddrR, 0);
		if (ReCode != ERROR_CODE_OK) {
			printk("Write REG_ScCbAddrR Failed!\n");
			goto TEST_ERR;
		}

		ReCode = GetTxSC_CB(g_ScreenSetParam.iTxNum + g_ScreenSetParam.iRxNum + 128, m_ucTempData);
		if (ReCode != ERROR_CODE_OK) {
			printk("GetTxSC_CB Failed!\n");
			goto TEST_ERR;
		}
		for (index = 0; index < g_ScreenSetParam.iRxNum; ++index) {
			m_RawData[2 + g_ScreenSetParam.iTxNum][index] = m_ucTempData[index];
		}
		for (index = 0; index < g_ScreenSetParam.iTxNum; ++index) {
			m_RawData[3 + g_ScreenSetParam.iTxNum][index] = m_ucTempData[index + g_ScreenSetParam.iRxNum];
		}

		if (ReCode != ERROR_CODE_OK) {
			printk("Failed to Get SCap CB!\n");
		}
	}

	if (ReCode != ERROR_CODE_OK)	goto TEST_ERR;




	bFlag = GetTestCondition(WT_NeedProofOnTest, wc_value);
	if (g_stCfg_FT5X22_BasicThreshold.SCapCbTest_SetWaterproof_ON && bFlag) {
		printk("SCapCbTest in WaterProof On Mode:  \n");

		iMax = -m_RawData[0+g_ScreenSetParam.iTxNum][0];
		iMin = 2 * m_RawData[0+g_ScreenSetParam.iTxNum][0];
		iAvg = 0;
		Value = 0;
		iCount = 0;


		bFlag = GetTestCondition(WT_NeedRxOnVal, wc_value);
		if (bFlag)
			printk("SCap CB_Rx:  \n");
		for (i = 0; bFlag && i < g_ScreenSetParam.iRxNum; i++) {
			if (g_stCfg_MCap_DetailThreshold.InvalidNode_SC[0][i] == 0)
				continue;
			CBMin = g_stCfg_MCap_DetailThreshold.SCapCbTest_ON_Min[0][i];
			CBMax = g_stCfg_MCap_DetailThreshold.SCapCbTest_ON_Max[0][i];
			Value = m_RawData[0+g_ScreenSetParam.iTxNum][i];
			iAvg += Value;

			if (iMax < Value)
				iMax = Value;
			if (iMin > Value)
				iMin = Value;
			if (Value > CBMax || Value < CBMin) {
				btmpresult = false;
				printk("Failed. Num = %d, Value = %d, range = (%d, %d):\n", i+1, Value, CBMin, CBMax);
			}
			iCount++;
		}


		bFlag = GetTestCondition(WT_NeedTxOnVal, wc_value);
		if (bFlag)
			printk("SCap CB_Tx:  \n");
		for (i = 0; bFlag &&  i < g_ScreenSetParam.iTxNum; i++) {
			if (g_stCfg_MCap_DetailThreshold.InvalidNode_SC[1][i] == 0)
				continue;
			CBMin = g_stCfg_MCap_DetailThreshold.SCapCbTest_ON_Min[1][i];
			CBMax = g_stCfg_MCap_DetailThreshold.SCapCbTest_ON_Max[1][i];
			Value = m_RawData[1+g_ScreenSetParam.iTxNum][i];
			iAvg += Value;
			if (iMax < Value)
				iMax = Value;
			if (iMin > Value)
				iMin = Value;
			if (Value > CBMax || Value < CBMin) {
				btmpresult = false;
				printk("Failed. Num = %d, Value = %d, range = (%d, %d):\n", i+1, Value, CBMin, CBMax);
			}
			iCount++;
		}

		if (0 == iCount) {
			iAvg = 0;
			iMax = 0;
			iMin = 0;
		} else
			iAvg = iAvg/iCount;

		printk("SCap CB in Waterproof-ON, Max : %d, Min: %d, Deviation: %d, Average: %d\n", iMax, iMin, iMax - iMin, iAvg);

		ibiggerValue = g_ScreenSetParam.iTxNum > g_ScreenSetParam.iRxNum?g_ScreenSetParam.iTxNum:g_ScreenSetParam.iRxNum;
		Save_Test_Data(m_RawData, g_ScreenSetParam.iTxNum+0, 2, ibiggerValue, 1);
	}

	bFlag = GetTestCondition(WT_NeedProofOffTest, wc_value);
	if (g_stCfg_FT5X22_BasicThreshold.SCapCbTest_SetWaterproof_OFF && bFlag) {
		printk("SCapCbTest in WaterProof OFF Mode:  \n");
		iMax = -m_RawData[2+g_ScreenSetParam.iTxNum][0];
		iMin = 2 * m_RawData[2+g_ScreenSetParam.iTxNum][0];
		iAvg = 0;
		Value = 0;
		iCount = 0;


		bFlag = GetTestCondition(WT_NeedRxOffVal, wc_value);
		if (bFlag)
			printk("SCap CB_Rx:  \n");
		for (i = 0; bFlag &&  i < g_ScreenSetParam.iRxNum; i++) {
			if (g_stCfg_MCap_DetailThreshold.InvalidNode_SC[0][i] == 0)
				continue;
			CBMin = g_stCfg_MCap_DetailThreshold.SCapCbTest_OFF_Min[0][i];
			CBMax = g_stCfg_MCap_DetailThreshold.SCapCbTest_OFF_Max[0][i];
			Value = m_RawData[2+g_ScreenSetParam.iTxNum][i];
			iAvg += Value;

			if (iMax < Value)
				iMax = Value;
			if (iMin > Value)
				iMin = Value;
			if (Value > CBMax || Value < CBMin) {
				btmpresult = false;
				printk("Failed. Num = %d, Value = %d, range = (%d, %d):\n", i+1, Value, CBMin, CBMax);
			}
			iCount++;
		}


		bFlag = GetTestCondition(WT_NeedTxOffVal, wc_value);
		if (bFlag)
			printk("SCap CB_Tx:  \n");
		for (i = 0; bFlag && i < g_ScreenSetParam.iTxNum; i++) {

			if (g_stCfg_MCap_DetailThreshold.InvalidNode_SC[1][i] == 0)
				continue;
			CBMin = g_stCfg_MCap_DetailThreshold.SCapCbTest_OFF_Min[1][i];
			CBMax = g_stCfg_MCap_DetailThreshold.SCapCbTest_OFF_Max[1][i];
			Value = m_RawData[3+g_ScreenSetParam.iTxNum][i];

			iAvg += Value;
			if (iMax < Value)
				iMax = Value;
			if (iMin > Value)
				iMin = Value;
			if (Value > CBMax || Value < CBMin) {
				btmpresult = false;
				printk("Failed. Num = %d, Value = %d, range = (%d, %d):\n", i+1, Value, CBMin, CBMax);
			}
			iCount++;
		}

		if (0 == iCount) {
			iAvg = 0;
			iMax = 0;
			iMin = 0;
		} else
			iAvg = iAvg/iCount;

		printk("SCap CB in Waterproof-OFF, Max : %d, Min: %d, Deviation: %d, Average: %d\n", iMax, iMin, iMax - iMin, iAvg);

		ibiggerValue = g_ScreenSetParam.iTxNum > g_ScreenSetParam.iRxNum?g_ScreenSetParam.iTxNum:g_ScreenSetParam.iRxNum;
		Save_Test_Data(m_RawData, g_ScreenSetParam.iTxNum+2, 2, ibiggerValue, 2);
	}

	if (m_bV3TP) {
		ReCode = ReadReg(REG_MAPPING_SWITCH, &ucValue);
		if (ReCode != ERROR_CODE_OK) {
			printk("\n Read REG_MAPPING_SWITCH error. Error Code: %d\n", ReCode);
			goto TEST_ERR;
		}

		if (0 != ucValue) {
			ReCode = WriteReg(REG_MAPPING_SWITCH, 0);
			SysDelay(10);
			if (ReCode != ERROR_CODE_OK) {
				printk("Failed to switch mapping type!\n ");
				btmpresult = false;
			}
		}

		ReCode = GetChannelNum();
		if (ReCode != ERROR_CODE_OK) {
			printk("\n GetChannelNum error. Error Code: %d\n", ReCode);
			goto TEST_ERR;
		}
	}



	if (btmpresult) {
		*bTestResult = true;
		printk("\n\n//SCap CB Test Test is OK!\n");
	} else {
		*bTestResult = false;
		printk("\n\n//SCap CB Test Test is NG!\n");
	}
	return ReCode;

TEST_ERR:

	*bTestResult = false;
	printk("\n\n//SCap CB Test Test is NG!\n");
	return ReCode;
}

/************************************************************************
* Name: GetPanelRows(Same function name as FT_MultipleTest)
* Brief:  Get row of TP
* Input: none
* Output: pPanelRows
* Return: Comm Code. Code = 0x00 is OK, else fail.
***********************************************************************/
static unsigned char GetPanelRows(unsigned char *pPanelRows)
{
	return ReadReg(REG_TX_NUM, pPanelRows);
}

/************************************************************************
* Name: GetPanelCols(Same function name as FT_MultipleTest)
* Brief:  get column of TP
* Input: none
* Output: pPanelCols
* Return: Comm Code. Code = 0x00 is OK, else fail.
***********************************************************************/
static unsigned char GetPanelCols(unsigned char *pPanelCols)
{
	return ReadReg(REG_RX_NUM, pPanelCols);
}
/************************************************************************
* Name: StartScan(Same function name as FT_MultipleTest)
* Brief:  Scan TP, do it before read Raw Data
* Input: none
* Output: none
* Return: Comm Code. Code = 0x00 is OK, else fail.
***********************************************************************/
static int StartScan(void)
{
	unsigned char RegVal = 0;
	unsigned char times = 0;
	const unsigned char MaxTimes = 250;
	unsigned char ReCode = ERROR_CODE_COMM_ERROR;

	ReCode = ReadReg(DEVIDE_MODE_ADDR, &RegVal);
	if (ReCode == ERROR_CODE_OK) {
		RegVal |= 0x80;
		ReCode = WriteReg(DEVIDE_MODE_ADDR, RegVal);
		if (ReCode == ERROR_CODE_OK) {
			while (times++ < MaxTimes)
			{
				SysDelay(16);
				ReCode = ReadReg(DEVIDE_MODE_ADDR, &RegVal);
				if (ReCode == ERROR_CODE_OK) {
					if ((RegVal>>7) == 0)	break;
				} else {
					printk("StartScan read DEVIDE_MODE_ADDR error.\n");
					break;
				}
			}
			if (times < MaxTimes)	ReCode = ERROR_CODE_OK;
			else ReCode = ERROR_CODE_COMM_ERROR;
		} else
			printk("StartScan write DEVIDE_MODE_ADDR error.\n");
	} else
		printk("StartScan read DEVIDE_MODE_ADDR error.\n");
	return ReCode;

}
/************************************************************************
* Name: ReadRawData(Same function name as FT_MultipleTest)
* Brief:  read Raw Data
* Input: Freq(No longer used, reserved), LineNum, ByteNum
* Output: pRevBuffer
* Return: Comm Code. Code = 0x00 is OK, else fail.
***********************************************************************/
unsigned char ReadRawData(unsigned char Freq, unsigned char LineNum, int ByteNum, int *pRevBuffer) {
	unsigned char ReCode = ERROR_CODE_COMM_ERROR;
	unsigned char I2C_wBuffer[3];
	int i, iReadNum;
	unsigned short BytesNumInTestMode1 = 0;




	iReadNum = ByteNum/342;

	if (0 != (ByteNum%342)) iReadNum++;

	if (ByteNum <= 342) {
		BytesNumInTestMode1 = ByteNum;
	} else {
		BytesNumInTestMode1 = 342;
	}

	ReCode = WriteReg(REG_LINE_NUM, LineNum);

	if (ReCode != ERROR_CODE_OK) {
		printk("Failed to write REG_LINE_NUM! \n");
		goto READ_ERR;
	}


	I2C_wBuffer[0] = REG_RawBuf0;
	if (ReCode == ERROR_CODE_OK) {
		focal_msleep(10);
		ReCode = Comm_Base_IIC_IO(I2C_wBuffer, 1, m_ucTempData, BytesNumInTestMode1);
		if (ReCode != ERROR_CODE_OK) {
			printk("read rawdata Comm_Base_IIC_IO Failed!1 \n");
			goto READ_ERR;
		}
	}

	for (i = 1; i < iReadNum; i++) {
		if (ReCode != ERROR_CODE_OK) break;

		if (i == iReadNum-1) {
			focal_msleep(10);
			ReCode = Comm_Base_IIC_IO(NULL, 0, m_ucTempData+342*i, ByteNum-342*i);
			if (ReCode != ERROR_CODE_OK) {
				printk("read rawdata Comm_Base_IIC_IO Failed!2 \n");
				goto READ_ERR;
			}
		} else {
			focal_msleep(10);
			ReCode = Comm_Base_IIC_IO(NULL, 0, m_ucTempData+342*i, 342);

			if (ReCode != ERROR_CODE_OK) {
				printk("read rawdata Comm_Base_IIC_IO Failed!3 \n");
				goto READ_ERR;
			}
		}

	}

	if (ReCode == ERROR_CODE_OK) {
		for (i = 0; i < (ByteNum>>1); i++) {
			pRevBuffer[i] = (m_ucTempData[i<<1]<<8)+m_ucTempData[(i<<1)+1];
			�з���λ



		}
	}

READ_ERR:
	return ReCode;

}
/************************************************************************
* Name: GetTxSC_CB(Same function name as FT_MultipleTest)
* Brief:  get CB of Tx SCap
* Input: index
* Output: pcbValue
* Return: Comm Code. Code = 0x00 is OK, else fail.
***********************************************************************/
unsigned char GetTxSC_CB(unsigned char index, unsigned char *pcbValue)
{
	unsigned char ReCode = ERROR_CODE_OK;
	unsigned char wBuffer[4];

	if (index < 128) {
		*pcbValue = 0;
		WriteReg(REG_ScCbAddrR, index);
		ReCode = ReadReg(REG_ScCbBuf0, pcbValue);
	} else {
		WriteReg(REG_ScCbAddrR, 0);
		wBuffer[0] = REG_ScCbBuf0;
		ReCode = Comm_Base_IIC_IO(wBuffer, 1, pcbValue, index-128);

	}

	return ReCode;
}



/************************************************************************
* Name: AllocateMemory
* Brief:  Allocate pointer Memory
* Input: none
* Output: none
* Return: none
***********************************************************************/
static char pStoreMsgArea[1024 * 80] ;
static char pMsgAreaLine2[1024 * 80] ;
static char pStoreDataArea[1024 * 80];
static char pTmpBuff[1024 * 16] 	 ;

static int AllocateMemory(void)
{

	printk("AllocateMemory changed\n");
	g_pStoreMsgArea = NULL;
	if (NULL == g_pStoreMsgArea) {
		memset(pStoreMsgArea, 0, sizeof(pStoreMsgArea));
		g_pStoreMsgArea = pStoreMsgArea;;
		if (g_pStoreMsgArea == NULL) {
			printk("lancelot g_pStoreMsgArea malloc error.\n");
			goto ERR_0;
		}
	}
	g_pMsgAreaLine2 = NULL;
	if (NULL == g_pMsgAreaLine2) {
		memset(pMsgAreaLine2, 0, sizeof(pMsgAreaLine2));
		g_pMsgAreaLine2 = pMsgAreaLine2;
		if (g_pMsgAreaLine2 == NULL) {
			printk("lancelot g_pMsgAreaLine2 malloc error.\n");
			goto ERR_1;
		}
	}
	g_pStoreDataArea = NULL;
	if (NULL == g_pStoreDataArea) {
		memset(pStoreDataArea, 0, sizeof(pStoreDataArea));
		g_pStoreDataArea = pStoreDataArea;
		if (g_pStoreDataArea == NULL) {
			printk("lancelot g_pStoreDataArea malloc error.\n");
			goto ERR_2;
		}
	}

	g_pTmpBuff = NULL;
	if (NULL == g_pTmpBuff) {
		memset(pTmpBuff, 0, sizeof(pTmpBuff));
		g_pTmpBuff = pTmpBuff;
		if (g_pTmpBuff == NULL) {
			printk("lancelot g_pTmpBuff malloc error.\n");
			goto ERR_3;
		}
	}

	return 1;

ERR_3:
	g_pStoreDataArea = NULL;
	printk("pTmpBuff fail\n");
ERR_2:
	g_pMsgAreaLine2 = NULL;
	printk("pStoreDataArea fail\n");
ERR_1:
	g_pStoreMsgArea = NULL;
	printk("pMsgAreaLine fail\n");

ERR_0:
	printk("pStoreMsgArea fail\n");


	return -EPERM;

}
/************************************************************************
* Name: InitStoreParamOfTestData
* Brief:  Init store param of test data
* Input: none
* Output: none
* Return: none
***********************************************************************/
static void InitStoreParamOfTestData(void)
{


	g_lenStoreMsgArea = 0;

	g_lenStoreMsgArea += sprintf(g_pStoreMsgArea, "ECC, 85, 170, IC Name, %s, IC Code, %x\n", g_strIcName,  g_ScreenSetParam.iSelectedIC);


	g_lenMsgAreaLine2 = 0;



	g_lenStoreDataArea = 0;
	m_iStartLine = 11;

	m_iTestDataCount = 0;
}
/************************************************************************
* Name: MergeAllTestData
* Brief:  Merge All Data of test result
* Input: none
* Output: none
* Return: none
***********************************************************************/
static void MergeAllTestData(void)
{
	int iLen = 0;


	iLen = sprintf(g_pTmpBuff, "TestItem, %d, ", m_iTestDataCount);
	memcpy(g_pStoreMsgArea+g_lenStoreMsgArea, g_pTmpBuff, iLen);
	g_lenStoreMsgArea += iLen;


	memcpy(g_pStoreMsgArea+g_lenStoreMsgArea, g_pMsgAreaLine2, g_lenMsgAreaLine2);
	g_lenStoreMsgArea += g_lenMsgAreaLine2;


	iLen = sprintf(g_pTmpBuff, "\n\n\n\n\n\n\n\n\n");
	memcpy(g_pStoreMsgArea+g_lenStoreMsgArea, g_pTmpBuff, iLen);
	g_lenStoreMsgArea += iLen;


	memcpy(g_pStoreAllData, g_pStoreMsgArea, g_lenStoreMsgArea);


	if (0 != g_lenStoreDataArea) {
		memcpy(g_pStoreAllData+g_lenStoreMsgArea, g_pStoreDataArea, g_lenStoreDataArea);
	}

	printk("[focal] %s lenStoreMsgArea=%d,  lenStoreDataArea = %d\n", __func__, g_lenStoreMsgArea, g_lenStoreDataArea);
}


/************************************************************************
* Name: Save_Test_Data
* Brief:  Storage format of test data
* Input: int iData[TX_NUM_MAX][RX_NUM_MAX], int iArrayIndex, unsigned char Row, unsigned char Col, unsigned char ItemCount
* Output: none
* Return: none
***********************************************************************/
static void Save_Test_Data(int iData[TX_NUM_MAX][RX_NUM_MAX], int iArrayIndex, unsigned char Row, unsigned char Col, unsigned char ItemCount) {
	int iLen = 0;
	int i = 0, j = 0;


	iLen = sprintf(g_pTmpBuff, "NA, %d, %d, %d, %d, %d, ", \
		m_ucTestItemCode, Row, Col, m_iStartLine, ItemCount);
	memcpy(g_pMsgAreaLine2+g_lenMsgAreaLine2, g_pTmpBuff, iLen);
	g_lenMsgAreaLine2 += iLen;

	m_iStartLine += Row;
	m_iTestDataCount++;


	for (i = 0+iArrayIndex; i < Row+iArrayIndex; i++) {
		for (j = 0; j < Col; j++) {
			if (j == (Col - 1))
				iLen = sprintf(g_pTmpBuff, "%d, \n", iData[i][j]);
			else
				iLen = sprintf(g_pTmpBuff, "%d, ", iData[i][j]);

			memcpy(g_pStoreDataArea+g_lenStoreDataArea, g_pTmpBuff, iLen);
			g_lenStoreDataArea += iLen;
		}
	}

}

/************************************************************************
* Name: GetChannelNum
* Brief:  Get Channel Num(Tx and Rx)
* Input: none
* Output: none
* Return: Comm Code. Code = 0x00 is OK, else fail.
***********************************************************************/
static unsigned char GetChannelNum(void)
{
	unsigned char ReCode;
	unsigned char rBuffer[1];


	ReCode = GetPanelRows(rBuffer);
	if (ReCode == ERROR_CODE_OK) {
		g_ScreenSetParam.iTxNum = rBuffer[0];
		if (g_ScreenSetParam.iTxNum > g_ScreenSetParam.iUsedMaxTxNum) {
			printk("Failed to get Tx number, Get num = %d, UsedMaxNum = %d\n",
				g_ScreenSetParam.iTxNum, g_ScreenSetParam.iUsedMaxTxNum);
			return ERROR_CODE_INVALID_PARAM;
		}
	} else {
		printk("Failed to get Tx number\n");
	}



	ReCode = GetPanelCols(rBuffer);
	if (ReCode == ERROR_CODE_OK) {
		g_ScreenSetParam.iRxNum = rBuffer[0];
		if (g_ScreenSetParam.iRxNum > g_ScreenSetParam.iUsedMaxRxNum) {
			printk("Failed to get Rx number, Get num = %d, UsedMaxNum = %d\n",
				g_ScreenSetParam.iRxNum, g_ScreenSetParam.iUsedMaxRxNum);
			return ERROR_CODE_INVALID_PARAM;
		}
	} else {
		printk("Failed to get Rx number\n");
	}

	return ReCode;

}
/************************************************************************
* Name: GetRawData
* Brief:  Get Raw Data of MCAP
* Input: none
* Output: none
* Return: Comm Code. Code = 0x00 is OK, else fail.
***********************************************************************/
static unsigned char GetRawData(void)
{
	unsigned char ReCode = ERROR_CODE_OK;
	int iRow = 0;
	int iCol = 0;


	ReCode = EnterFactory();
	if (ERROR_CODE_OK != ReCode) {
		printk("Failed to Enter Factory Mode...\n");
		return ReCode;
	}



	if (0 == (g_ScreenSetParam.iTxNum + g_ScreenSetParam.iRxNum)) {
		ReCode = GetChannelNum();
		if (ERROR_CODE_OK != ReCode) {
			printk("Error Channel Num...\n");
			return ERROR_CODE_INVALID_PARAM;
		}
	}


	printk("Start Scan ...\n");
	ReCode = StartScan();
	if (ERROR_CODE_OK != ReCode) {
		printk("Failed to Scan ...\n");
		return ReCode;
	}


	memset(m_RawData, 0, sizeof(m_RawData));
	ReCode = ReadRawData(1, 0xAA, (g_ScreenSetParam.iTxNum * g_ScreenSetParam.iRxNum)*2, m_iTempRawData);
	for (iRow = 0; iRow < g_ScreenSetParam.iTxNum; iRow++) {
		for (iCol = 0; iCol < g_ScreenSetParam.iRxNum; iCol++) {
			m_RawData[iRow][iCol] = m_iTempRawData[iRow*g_ScreenSetParam.iRxNum + iCol];
		}
	}
	return ReCode;
}
/************************************************************************
* Name: ShowRawData
* Brief:  Show RawData
* Input: none
* Output: none
* Return: none.
***********************************************************************/
static void ShowRawData(void)
{
	int iRow, iCol;

	for (iRow = 0; iRow < g_ScreenSetParam.iTxNum; iRow++) {
		printk("\nTx%2d:  ", iRow+1);
		for (iCol = 0; iCol < g_ScreenSetParam.iRxNum; iCol++) {
			printk("%5d    ", m_RawData[iRow][iCol]);
		}
	}
}

/************************************************************************
* Name: GetChannelNumNoMapping
* Brief:  get Tx&Rx num from other Register
* Input: none
* Output: none
* Return: Comm Code. Code = 0x00 is OK, else fail.
***********************************************************************/
static unsigned char GetChannelNumNoMapping(void)
{
	unsigned char ReCode;
	unsigned char rBuffer[1];


	printk("Get Tx Num...\n");
	ReCode = ReadReg(REG_TX_NOMAPPING_NUM,  rBuffer);
	if (ReCode == ERROR_CODE_OK) {
		g_ScreenSetParam.iTxNum = rBuffer[0];
	} else {
		printk("Failed to get Tx number\n");
	}


	printk("Get Rx Num...\n");
	ReCode = ReadReg(REG_RX_NOMAPPING_NUM,  rBuffer);
	if (ReCode == ERROR_CODE_OK) {
		g_ScreenSetParam.iRxNum = rBuffer[0];
	} else {
		printk("Failed to get Rx number\n");
	}

	return ReCode;
}
/************************************************************************
* Name: SwitchToNoMapping
* Brief:  If it is V3 pattern, Get Tx/Rx Num again
* Input: none
* Output: none
* Return: Comm Code. Code = 0x00 is OK, else fail.
***********************************************************************/
static unsigned char SwitchToNoMapping(void)
{
	unsigned char chPattern = -1;
	unsigned char ReCode = ERROR_CODE_OK;
	unsigned char RegData = -1;
	ReCode = ReadReg(REG_PATTERN_5422, &chPattern);
	if (ReCode != ERROR_CODE_OK) {
		printk("Switch To NoMapping Failed!\n");
		goto READ_ERR;
	}

	if (1 == chPattern) {
		RegData = -1;
		ReCode = ReadReg(REG_MAPPING_SWITCH, &RegData);
		if (ReCode != ERROR_CODE_OK) {
			printk("read REG_MAPPING_SWITCH Failed!\n");
			goto READ_ERR;
		}

		if (1 != RegData) {
			ReCode = WriteReg(REG_MAPPING_SWITCH, 1);
			if (ReCode != ERROR_CODE_OK) {
				printk("write REG_MAPPING_SWITCH Failed!\n");
				goto READ_ERR;
			}
			focal_msleep(20);
			ReCode = GetChannelNumNoMapping();

			if (ReCode != ERROR_CODE_OK) {
				printk("GetChannelNumNoMapping Failed!\n");
				goto READ_ERR;
			}
		}
	}

READ_ERR:
	return ReCode;
}
/************************************************************************
* Name: GetTestCondition
* Brief:  Check whether Rx or TX need to test, in Waterproof ON/OFF Mode.
* Input: none
* Output: none
* Return: true: need to test; false: Not tested.
***********************************************************************/
static boolean GetTestCondition(int iTestType, unsigned char ucChannelValue)
{
	boolean bIsNeeded = false;
	switch (iTestType) {
	case WT_NeedProofOnTest:
		bIsNeeded = !(ucChannelValue & 0x20);
		break;
	case WT_NeedProofOffTest:
		bIsNeeded = !(ucChannelValue & 0x80);
		break;
	case WT_NeedTxOnVal:
		bIsNeeded = !(ucChannelValue & 0x40) || !(ucChannelValue & 0x04);
		break;
	case WT_NeedRxOnVal:
		bIsNeeded = !(ucChannelValue & 0x40) || (ucChannelValue & 0x04);
		break;
	case WT_NeedTxOffVal:
		bIsNeeded = (0x00 == (ucChannelValue & 0x03)) || (0x02 == (ucChannelValue & 0x03));
		break;
	case WT_NeedRxOffVal:
		bIsNeeded = (0x01 == (ucChannelValue & 0x03)) || (0x02 == (ucChannelValue & 0x03));
		break;
	default:
		break;
	}
	return bIsNeeded;
}


