/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "14a_init_anticol.h"

uint8_t k14a_init_anticol_optimized_buffer[20];

uint8_t k14a_anticol_opitimized_recv()
{
    uint8_t level = 0x00;
    g_ui8_cb14a_Buffer = TRF7970A_SPI_Read_SingleRegister(TRF79X0_FIFO_STATUS_REG);
    if((g_ui8_cb14a_Buffer == 2) || (g_ui8_cb14a_Buffer == 9))
    {
        level = TRF7970A_SPI_Read_SingleRegister(TRF79X0_FIFO_REG);
    }

    return level;
}

bool k14a_anticol_loop() // we try to know by IRQ Status and fifo size before to know if we must enter here
{
    bool ret = false;
    uint8_t cbData = 0;
    const uint8_t *pcbData;
    uint8_t level, bIsCRC;

    /*
     * Send ATQA
     */
    __delay_cycles(K14A_ANTICOL_GENERIC_DELAY_CYCLES + 120);
    k14a_Send(k14a_init_anticol_optimized_buffer + 0, 2, false);
    do
    {
        pcbData = 0;
        if(TRF7970A_SPI_waitIrq() & TRF79X0_IRQ_STATUS_SRX)
        {
            level = k14a_anticol_opitimized_recv();
            if(level)
            {
                if (g_ui8_cb14a_Buffer == 2)
                {
                    if(level == SDD_REQ_CL1)
                    {
                        pcbData = k14a_init_anticol_optimized_buffer + 2;
                        __delay_cycles(K14A_ANTICOL_GENERIC_DELAY_CYCLES + 20);
                    }
                    else if(level == SDD_REQ_CL2)
                    {
                        pcbData = k14a_init_anticol_optimized_buffer + 7;
                        __delay_cycles(K14A_ANTICOL_GENERIC_DELAY_CYCLES + 10);
                    }
                    else if(level == SDD_REQ_CL3)
                    {
                        pcbData = k14a_init_anticol_optimized_buffer + 12;
                        __delay_cycles(K14A_ANTICOL_GENERIC_DELAY_CYCLES + 0);
                    }

                    if(pcbData)
                    {
                        cbData = 5;
                        bIsCRC = false;
                    }
                }
                else
                {
                    if(level == SDD_REQ_CL1)
                    {
                        pcbData = k14a_init_anticol_optimized_buffer + 17;
                        __delay_cycles(K14A_ANTICOL_GENERIC_DELAY_CYCLES + 220);
                    }
                    else if(level == SDD_REQ_CL2)
                    {
                        pcbData = k14a_init_anticol_optimized_buffer + 18;
                        __delay_cycles(K14A_ANTICOL_GENERIC_DELAY_CYCLES + 210);
                    }
                    else if(level == SDD_REQ_CL3)
                    {
                        pcbData = k14a_init_anticol_optimized_buffer + 19;
                        __delay_cycles(K14A_ANTICOL_GENERIC_DELAY_CYCLES + 200);
                    }

                    if(pcbData)
                    {
                        cbData = 1;
                        bIsCRC = true;
                    }
                }

                if (pcbData)
                {
                    if(k14a_Send(pcbData, cbData, bIsCRC))
                    {
                        if(bIsCRC && !(*pcbData & 0x04))
                        {
                            pcbData = 0;
                            ret = true;
                        }
                    }
                    else
                    {
                        pcbData = 0;
                    }
                }
            }
            else
            {
                TRF7970A_SPI_DirectCommand(TRF79X0_RESET_FIFO_CMD);
            }
        }

    } while(pcbData);

    return ret;
}

void k14a_anticol_prepare_buffer(const T3A_INFOS *pt3a_infos)
{
    k14a_init_anticol_optimized_buffer[0] = pt3a_infos->ATQA[0];
    k14a_init_anticol_optimized_buffer[1] = pt3a_infos->ATQA[1];

    if(pt3a_infos->tUID == T3A_UID_4BYTES)
    {
        k14a_init_anticol_optimized_buffer[2] = pt3a_infos->UID[0];
        k14a_init_anticol_optimized_buffer[3] = pt3a_infos->UID[1];
        k14a_init_anticol_optimized_buffer[4] = pt3a_infos->UID[2];
        k14a_init_anticol_optimized_buffer[5] = pt3a_infos->UID[3];

        k14a_init_anticol_optimized_buffer[17] = pt3a_infos->SAK;
    }
    else
    {
        k14a_init_anticol_optimized_buffer[2] = CASCADE_TAG;
        k14a_init_anticol_optimized_buffer[3] = pt3a_infos->UID[0];
        k14a_init_anticol_optimized_buffer[4] = pt3a_infos->UID[1];
        k14a_init_anticol_optimized_buffer[5] = pt3a_infos->UID[2];

        k14a_init_anticol_optimized_buffer[17] = pt3a_infos->SAK | 0x04;

        if(pt3a_infos->tUID == T3A_UID_7BYTES)
        {
            k14a_init_anticol_optimized_buffer[7] = pt3a_infos->UID[3];
            k14a_init_anticol_optimized_buffer[8] = pt3a_infos->UID[4];
            k14a_init_anticol_optimized_buffer[9] = pt3a_infos->UID[5];
            k14a_init_anticol_optimized_buffer[10] = pt3a_infos->UID[6];

            k14a_init_anticol_optimized_buffer[18] = pt3a_infos->SAK;
        }
        else // T3A_UID_10BYTES
        {
            k14a_init_anticol_optimized_buffer[7] = CASCADE_TAG;
            k14a_init_anticol_optimized_buffer[8] = pt3a_infos->UID[3];
            k14a_init_anticol_optimized_buffer[9] = pt3a_infos->UID[4];
            k14a_init_anticol_optimized_buffer[10] = pt3a_infos->UID[5];

            k14a_init_anticol_optimized_buffer[18] = pt3a_infos->SAK | 0x04;

            k14a_init_anticol_optimized_buffer[12] = pt3a_infos->UID[6];
            k14a_init_anticol_optimized_buffer[13] = pt3a_infos->UID[7];
            k14a_init_anticol_optimized_buffer[14] = pt3a_infos->UID[8];
            k14a_init_anticol_optimized_buffer[15] = pt3a_infos->UID[9];

            k14a_init_anticol_optimized_buffer[19] = pt3a_infos->SAK;

            k14a_init_anticol_optimized_buffer[16] = k14a_init_anticol_optimized_buffer[12] ^ k14a_init_anticol_optimized_buffer[13] ^ k14a_init_anticol_optimized_buffer[14] ^ k14a_init_anticol_optimized_buffer[15];
        }
        k14a_init_anticol_optimized_buffer[11] = k14a_init_anticol_optimized_buffer[7] ^ k14a_init_anticol_optimized_buffer[8] ^ k14a_init_anticol_optimized_buffer[9] ^ k14a_init_anticol_optimized_buffer[10];
    }
    k14a_init_anticol_optimized_buffer[6] = k14a_init_anticol_optimized_buffer[2] ^ k14a_init_anticol_optimized_buffer[3] ^ k14a_init_anticol_optimized_buffer[4] ^ k14a_init_anticol_optimized_buffer[5];
}
