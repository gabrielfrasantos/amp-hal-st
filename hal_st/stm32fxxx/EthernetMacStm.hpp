#ifndef HAL_ETHERNET_MAC_STM_HPP
#define HAL_ETHERNET_MAC_STM_HPP

#include DEVICE_HEADER
#include "generated/stm32fxxx/PeripheralTable.hpp"
#include "hal/interfaces/Ethernet.hpp"
#include "hal_st/cortex/InterruptCortex.hpp"
#include "infra/util/Optional.hpp"
#include <array>

#if defined(HAS_PERIPHERAL_ETHERNET)

namespace hal
{
    class EthernetMacStm
        : public EthernetMac
    {
    public:
        EthernetMacStm(EthernetSmi& ethernetSmi, LinkSpeed linkSpeed, MacAddress macAddress);
        ~EthernetMacStm();

        void Initialize() override;
        void SendBuffer(infra::ConstByteRange data, bool last) override;
        void RetryAllocation() override;
        void AddMacAddressFilter(MacAddress address) override;
        void RemoveMacAddressFilter(MacAddress address) override;

    private:
        void ResetDma();
        void Interrupt();

    private:
        class ReceiveDescriptors
        {
        public:
            ReceiveDescriptors(EthernetMacStm& ethernetMac);

            void ReceivedFrame();
            void RequestReceiveBuffers();

        private:
            bool RequestReceiveBuffer();

        private:
            EthernetMacStm& ethernetMac;

            alignas(uint64_t) std::array<ETH_DMADescTypeDef, 8> descriptors;

            uint32_t receiveDescriptorAllocatedIndex = 0;
            uint32_t receiveDescriptorReceiveIndex = 0;
            uint32_t receivedFrameBuffers = 0;
            uint8_t receivedFramesAllocated = 0;
        };

        class SendDescriptors
        {
        public:
            SendDescriptors(EthernetMacStm& ethernetMac);

            void SendBuffer(infra::ConstByteRange data, bool last);
            void SentFrame();

        private:
            EthernetMacStm& ethernetMac;

            alignas(uint64_t) std::array<ETH_DMADescTypeDef, 12> descriptors;

            uint32_t sendDescriptorIndexFirst = 0;
            uint32_t sendDescriptorIndex = 0;
            bool sendFirst = true;
        };

    private:
        EthernetSmi& ethernetSmi;
        MacAddress macAddress;
        DispatchedInterruptHandler interrupt;

        infra::Optional<ReceiveDescriptors> receiveDescriptors;
        infra::Optional<SendDescriptors> sendDescriptors;
    };
}

#endif

#endif
