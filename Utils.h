#pragma once

namespace Utils {
	template <typename T>
	void Write(__int64 address, T value) {
		DWORD oldProtection;
		VirtualProtect(reinterpret_cast<void**>(address), sizeof(T), PAGE_EXECUTE_READWRITE, &oldProtection);
		*reinterpret_cast<T*>(address) = value;
		VirtualProtect(reinterpret_cast<void**>(address), sizeof(T), oldProtection, &oldProtection);
	}

	template <typename K>
	K Read(__int64 address) {
		return *reinterpret_cast<K*>(address);
	}

    uint32_t CalculateChecksum(const uint8_t* data, size_t length) {
        uint32_t checksum = 0;
        for (size_t i = 0; i < length; i++) {
            checksum ^= data[i];
            checksum += data[i];
        }
        return checksum;
    }

    uint32_t GetTextSectionChecksum(uint64_t hModule) {
        PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hModule;
        if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
            return 0;
        }

        PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((uint8_t*)hModule + dosHeader->e_lfanew);
        if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
            return 0;
        }

        PIMAGE_SECTION_HEADER sectionHeader = IMAGE_FIRST_SECTION(ntHeaders);
        for (WORD i = 0; i < ntHeaders->FileHeader.NumberOfSections; ++i, ++sectionHeader) {
            char sectionName[IMAGE_SIZEOF_SHORT_NAME + 1] = { 0 };
            memcpy(sectionName, sectionHeader->Name, IMAGE_SIZEOF_SHORT_NAME);

            if (strcmp(sectionName, ".text") == 0) {
                uint8_t* sectionData = (uint8_t*)hModule + sectionHeader->VirtualAddress;
                return CalculateChecksum(sectionData, sectionHeader->Misc.VirtualSize);
            }
        }

        return 0;
    }
}