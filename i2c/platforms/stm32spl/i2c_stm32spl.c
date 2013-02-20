#include <bm/i2c.h>
#include <bm/delay.h>
#include <errno.h>

#include <stm32f10x.h>
#include <stm32f10x_i2c.h>

int i2c_stm32spl_WaitForEventTimeout(I2C_TypeDef *I2Cx, uint32_t event, uint16_t mS)
{
    uint16_t tick_stop = get_tick_count() + mS;
    int state = 0;
    while (get_tick_count() < tick_stop)
    {
        state = I2C_CheckEvent(I2Cx, event);
        if (state)
            break;
        system_nop();
    }

    if (!state)
    {
        I2C_GenerateSTOP(I2Cx, ENABLE);
        return -ETIMEDOUT;
    }
    return 0;
}

int i2c_transfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num)
{
    I2C_TypeDef *I2CDevice = (I2C_TypeDef *)adap->impl_data;

    BM_INIT_TIMEOUT_WAIT();

    BM_TIMEOUT_WAIT_MS(I2C_GetFlagStatus(I2CDevice, I2C_FLAG_BUSY), adap->timeout)
    {
        return -EBUSY;
    }

    int i;
    for (i = 0; i < num; i++)
    {
        I2C_GenerateSTART(I2CDevice, ENABLE);
        if (i2c_stm32spl_WaitForEventTimeout(I2CDevice, I2C_EVENT_MASTER_MODE_SELECT, adap->timeout))
            return -EAGAIN;

        I2C_Send7bitAddress(I2CDevice, (msgs[i].addr << 1), (msgs[i].flags & I2C_MSG_READ) ? I2C_Direction_Receiver : I2C_Direction_Transmitter);
        if (i2c_stm32spl_WaitForEventTimeout(I2CDevice, (msgs[i].flags & I2C_MSG_READ) ? I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED : I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, adap->timeout))
            return -ENODEV;

        if (msgs[i].flags & I2C_MSG_READ)
        {
            for (int j = 0; j < msgs[i].len; j++)
            {
                if (j == msgs[i].len - 1)
                {
                    I2C_AcknowledgeConfig(I2CDevice, DISABLE);
                    if (i == num - 1)
                        I2C_GenerateSTOP(I2CDevice, ENABLE);
                }
                BM_TIMEOUT_WAIT_MS(I2C_GetFlagStatus(I2CDevice, I2C_FLAG_RXNE) == RESET, adap->timeout)
                {
                    I2C_GenerateSTOP(I2CDevice, ENABLE);
                    return -ETIMEDOUT;
                }
                if (j == msgs[i].len - 1)
                {
                    I2C_AcknowledgeConfig(I2CDevice, ENABLE);
                }
                msgs[i].buf[j] = I2C_ReceiveData(I2CDevice) & 0xff;
            }
        }
        else
        {
            for (int j = 0; j < msgs[i].len; j++)
            {
                I2C_SendData(I2CDevice, msgs[i].buf[j]);
                BM_TIMEOUT_WAIT_MS(I2C_GetFlagStatus(I2CDevice, I2C_FLAG_BTF) == RESET, adap->timeout)
                {
                    I2C_GenerateSTOP(I2CDevice, ENABLE);
                    return -ETIMEDOUT;
                }
                if ((i == num - 1) && (j == msgs[i].len - 1))
                    I2C_GenerateSTOP(I2CDevice, ENABLE);
            }
        }
    }
    return 0;
}


int i2c_init_adapter(struct i2c_adapter *adap)
{
    I2C_TypeDef *I2CDevice = (I2C_TypeDef *)adap->impl_data;

    I2C_InitTypeDef conf;

    I2C_DeInit(I2CDevice);

    conf.I2C_Mode = I2C_Mode_I2C;
    conf.I2C_DutyCycle = I2C_DutyCycle_2;
    conf.I2C_OwnAddress1 = 0x00;
    conf.I2C_Ack = I2C_Ack_Enable;
    conf.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    conf.I2C_ClockSpeed = adap->speed;

    I2C_Init(I2CDevice, &conf);
    I2C_Cmd(I2CDevice, ENABLE);

    BM_INIT_TIMEOUT_WAIT();
    BM_TIMEOUT_WAIT_MS(I2C_GetFlagStatus(I2CDevice, I2C_FLAG_BUSY), adap->timeout)
    {
        return -EBUSY;
    }

    return 0;
}

int i2c_deinit_adapter(struct i2c_adapter *adap)
{
    I2C_TypeDef *I2CDevice = (I2C_TypeDef *)adap->impl_data;

    I2C_Cmd(I2CDevice, DISABLE);
    I2C_DeInit(I2CDevice);

    return 0;
}
