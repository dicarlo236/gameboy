//
// Created by jared on 1/10/19.
//

#include <assert.h>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include "mem.h"
#include "platform.h"

MemState globalMemState;

void CartInfo::print() {
  printf("Gameboy Cartridge\n"
         "\ttitle: %s\n"
         "\tisColor: 0x%x\n"
         "\tSGB: 0x%x\n"
         "\tcartType: 0x%x\n"
         "\tromSize: 0x%x\n"
         "\tramSize: 0x%x\n"
         "\tnotJapan: 0x%x\n",
         title, isColor, SGB, (u8)cartType, (u8)romSize, (u8)ramSize, notJapan);
}

void initMem(FileLoadData file) {
  CartInfo* cartInfo = (CartInfo*)(file.data + CART_INFO_ADDR);
  cartInfo->print();
  globalMemState.inBios = true;
  globalMemState.rom0 = file.data;
  globalMemState.mappedRom = nullptr;
  globalMemState.vram = nullptr;
  globalMemState.mappedRam = nullptr;
  globalMemState.internalRam = nullptr;
  globalMemState.upperRam = nullptr;
  globalMemState.spriteAttribute = nullptr;

  // allocate memories:
  // internal RAM
  globalMemState.internalRam = (u8*)malloc(0x2000);
  globalMemState.vram = (u8*)malloc(0x2000);
  globalMemState.upperRam    = (u8*)malloc(0x80);
  globalMemState.ioRegs      = (u8*)malloc(0x4c);

  // clear RAMs
  memset(globalMemState.internalRam, 0, 0x2000);
  memset(globalMemState.vram, 0, 0x2000);
  memset(globalMemState.upperRam, 0, 0x80);
  memset(globalMemState.ioRegs, 0, 0x4c);

  // setup i/o regs and friends
  u8* io = globalMemState.ioRegs;
  io[IO_TIMA] = 0; // reset TIMER COUNT to 0
  io[IO_TMA] = 0;  // TIMER RELOAD
  io[IO_TAC] = 0;  // TIMER STOP
  io[IO_NR10] = 0x80;
  io[IO_NR11] = 0xbf;
  io[IO_NR12] = 0xf3;
  io[IO_NR14] = 0xbf;
  io[IO_NR21] = 0x3f;
  io[IO_NR22] = 0x00;
  io[IO_NR24] = 0xbf;
  io[IO_NR30] = 0x7f;
  io[IO_NR31] = 0xff;
  io[IO_NR32] = 0x9f;
  io[IO_NR34] = 0xbf;
  io[IO_NR41] = 0xff;
  io[IO_NR42] = 0x00;
  io[IO_NR43] = 0x00;
  io[IO_NR44] = 0xbf;
  io[IO_NR50] = 0x77;
  io[IO_NR51] = 0xf3;
  io[IO_NR52] = 0xf1;
  io[IO_LCDC] = 0x91;
  io[IO_SCROLLY] = 0x00;
  io[IO_SCROLLX] = 0x00;
  io[IO_LYC] = 0x00;
  io[IO_BGP] = 0xfc;
  io[IO_OBP0] = 0xff;
  io[IO_OBP1] = 0xff;
  io[IO_WINY] = 0x00;
  io[IO_WINX] = 0x00;

  // turn off interrupts
  globalMemState.upperRam[0x7f] = 0;
}

void shutdownMem() {
  free(globalMemState.internalRam);
  free(globalMemState.upperRam);
  free(globalMemState.ioRegs);
  free(globalMemState.vram);
}

static u8 bios[256] = {0x31, 0xFE, 0xFF, // LD, SP, $fffe   0
                       0xAF,             // XOR A           3
                       0x21, 0xFF, 0x9F, // LD HL, $9fff    4
                       0x32,             // LD (HL--), A    7
                       0xCB, 0x7C,       // BIT 7, H        8
                       0x20, 0xFB,       // JR NZ 7         a
                       0x21, 0x26, 0xFF, // LD HL, $ff26    c
                       0x0E, 0x11,       // LD c,$11        f
                       0x3E, 0x80,       // LD a,$80        11
                       0x32,             // LD (HL--), A    13
                       0xE2,             // LD($FF00+C),A   14
                       0x0C,             // INC C           15
                       0x3E, 0xF3,       // LD A, $f3       16
                       0xE2,             // LD (HL--), A    18
                       0x32,             // LD($FF00+C),A   19
                       0x3E, 0x77,       // LD A,$77        1a
                       0x77, 0x3E, 0xFC, 0xE0,
                       0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1A, 0xCD, 0x95, 0x00, 0xCD, 0x96, 0x00, 0x13, 0x7B,
                       0xFE, 0x34, 0x20, 0xF3, 0x11, 0xD8, 0x00, 0x06, 0x08, 0x1A, 0x13, 0x22, 0x23, 0x05, 0x20, 0xF9,
                       0x3E, 0x19, 0xEA, 0x10, 0x99, 0x21, 0x2F, 0x99, 0x0E, 0x0C, 0x3D, 0x28, 0x08, 0x32, 0x0D, 0x20,
                       0xF9, 0x2E, 0x0F, 0x18, 0xF3, 0x67, 0x3E, 0x64, 0x57, 0xE0, 0x42, 0x3E, 0x91, 0xE0, 0x40, 0x04,
                       0x1E, 0x02, 0x0E, 0x0C, 0xF0, 0x44, 0xFE, 0x90, 0x20, 0xFA, 0x0D, 0x20, 0xF7, 0x1D, 0x20, 0xF2,
                       0x0E, 0x13, 0x24, 0x7C, 0x1E, 0x83, 0xFE, 0x62, 0x28, 0x06, 0x1E, 0xC1, 0xFE, 0x64, 0x20, 0x06,
                       0x7B, 0xE2, 0x0C, 0x3E, 0x87, 0xF2, 0xF0, 0x42, 0x90, 0xE0, 0x42, 0x15, 0x20, 0xD2, 0x05, 0x20,
                       0x4F, 0x16, 0x20, 0x18, 0xCB, 0x4F, 0x06, 0x04, 0xC5, 0xCB, 0x11, 0x17, 0xC1, 0xCB, 0x11, 0x17,
                       0x05, 0x20, 0xF5, 0x22, 0x23, 0x22, 0x23, 0xC9, 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
                       0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
                       0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
                       0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E, 0x3c, 0x42, 0xB9, 0xA5, 0xB9, 0xA5, 0x42, 0x4C,
                       0x21, 0x04, 0x01, 0x11, 0xA8, 0x00, 0x1A, 0x13, 0xBE, 0x20, 0xFE, 0x23, 0x7D, 0xFE, 0x34, 0x20,
                       0xF5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xFB, 0x86, 0x20, 0xFE, 0x3E, 0x01, 0xE0, 0x50};

u16 readU16(u16 addr) {
  return (u16)readByte(addr) + ((u16)(readByte(addr+(u16)1)) << 8);
}

void writeU16(u16 mem, u16 addr) {
  writeByte((u8)(mem & 0xff), addr);
  writeByte((u8)(mem >> 8),  addr + (u16)1);
}

u8 readByte(u16 addr) {
  switch(addr & 0xf000) {
    case 0x0000: // either BIOS or ROM 0:
      if(globalMemState.inBios) {
        if(addr < 0x100) {
          return bios[addr];
        } else if(addr == 0x100) {
          printf("EXIT BIOS!\n");
          globalMemState.inBios = false;
        } else {
          return globalMemState.rom0[addr];
        }
      } else {
        return globalMemState.rom0[addr];
      }

    case 0x1000: // ROM 0
    case 0x2000: // ROM 0
    case 0x3000: // ROM 0
      return globalMemState.rom0[addr];

    case 0x4000: // banked ROM
    case 0x5000:
    case 0x6000:
    case 0x7000:
      return globalMemState.mappedRom[addr & 0x3fff];

    case 0x8000: // VRAM
    case 0x9000:
      return globalMemState.vram[addr & 0x1fff];

    case 0xa000: // mapped RAM
    case 0xb000:
      return globalMemState.mappedRam[addr & 0x1fff];

    case 0xc000: // internal RAM
    case 0xd000:
      return globalMemState.internalRam[addr & 0x1fff];

    case 0xe000: // interal RAM copy
      return globalMemState.internalRam[addr & 0x1fff];

    case 0xf000: // either internal RAM copy or I/O or top-ram
      switch(addr & 0x0f00) {
        case 0x000:
        case 0x100:
        case 0x200:
        case 0x300:
        case 0x400:
        case 0x500:
        case 0x600:
        case 0x700:
        case 0x800:
        case 0x900:
        case 0xa00:
        case 0xb00:
        case 0xc00:
        case 0xd00:
          return globalMemState.internalRam[addr & 0x1fff];

        case 0xe00:
          if(addr < 0xfea0)
            return globalMemState.spriteAttribute[addr & 0xff];
          else
            return 0;

        case 0xf00:
          if(addr >= 0xff80) {
            return globalMemState.upperRam[addr & 0x7f];
          } else {
            u8 lowAddr = (u8)(addr & 0xff);
            switch(lowAddr) {
              case IO_LY:
              case IO_SCROLLX:
              case IO_SCROLLY:
              case IO_NR10:
              case IO_NR11:
              case IO_NR12:
              case IO_NR13:
              case IO_NR14:
              case IO_NR21:
              case IO_NR22:
              case IO_NR23:
              case IO_NR24:
              case IO_NR30:
              case IO_NR31:
              case IO_NR32:
              case IO_NR33:
              case IO_NR34:
              case IO_NR41:
              case IO_NR42:
              case IO_NR43:
              case IO_NR44:
              case IO_NR50:
              case IO_NR51:
              case IO_NR52:
              case IO_WAVE_PATTERN:
                return globalMemState.ioRegs[lowAddr];
                break;
              case IO_P1:
              case IO_SERIAL_SB:
              case IO_SERIAL_SC:
              case IO_DIV:
              case IO_TIMA:
              case IO_TMA:
              case IO_TAC:
              case IO_IF:

              case IO_LCDC:
              case IO_STAT:

              case IO_LYC:
              case IO_DMA:
              case IO_BGP:
              case IO_OBP0:
              case IO_OBP1:
              case IO_WINY:
              case IO_WINX:
                printf("unhandled I/O read @ 0x%x\n", addr);
                assert(false);
                break;
              default:
                printf("unknown I/O read @ 0x%x\n", addr);
                break;
            }

          }
        default:
          assert(false);

      }
      break;

    default:
      assert(false);
  }
}


void writeByte(u8 byte, u16 addr) {
  switch(addr & 0xf000) {
    case 0x0000: // ROM 0
      if(globalMemState.inBios) {
        printf("ERROR: tried to write into ROM0 or BIOS (@ 0x%04x) during BIOS!\n", addr);
        throw std::runtime_error("write");
      } else {
        printf("ERROR: unhanled write into ROM0\n");
        throw std::runtime_error("write");
      }
      break;
    case 0x1000: // ROM 0
    case 0x2000: // ROM 0
    case 0x3000: // ROM 0
      printf("ERROR: unhanled write into ROM0\n");
      throw std::runtime_error("write");
      break;


    case 0x4000: // ROM 1
    case 0x5000: // ROM 1
    case 0x6000: // ROM 1
    case 0x7000: // ROM 1
      printf("ERROR: unhanled write into ROM1\n");
      throw std::runtime_error("write");
      break;

    case 0x8000: // VRAM
    case 0x9000:
      globalMemState.vram[addr & 0x1fff] = byte;
      break;

    case 0xa000: // mapped RAM
    case 0xb000:
      globalMemState.mappedRam[addr & 0x1fff] = byte;
      break;

    case 0xc000: // internal RAM
    case 0xd000:
      globalMemState.internalRam[addr & 0x1fff] = byte;
      break;

    case 0xe000: // interal RAM copy
      globalMemState.internalRam[addr & 0x1fff] = byte;
      break;

    case 0xf000:
      switch(addr & 0x0f00) {
        case 0x000:
        case 0x100:
        case 0x200:
        case 0x300:
        case 0x400:
        case 0x500:
        case 0x600:
        case 0x700:
        case 0x800:
        case 0x900:
        case 0xa00:
        case 0xb00:
        case 0xc00:
        case 0xd00:
          globalMemState.internalRam[addr & 0x1fff] = byte;
          break;

        case 0xe00:
          printf("ERROR unhandled write into sprite attributes!\n");
          throw std::runtime_error("sprite write");
          break;

        case 0xf00:
          if(addr >= 0xff80) {
            globalMemState.upperRam[addr & 0x7f] = byte;
            break;
          } else {
            u16 maskedAddress = addr & 0x7f;
            globalMemState.ioRegs[maskedAddress] = byte;
            u8 lowAddr = (u8)(addr & 0xff);
            switch(lowAddr) {

              case IO_NR10:
              case IO_NR11:
              case IO_NR12:
              case IO_NR13:
              case IO_NR14:
              case IO_NR21:
              case IO_NR22:
              case IO_NR23:
              case IO_NR24:
              case IO_NR30:
              case IO_NR31:
              case IO_NR32:
              case IO_NR33:
              case IO_NR34:
              case IO_NR41:
              case IO_NR42:
              case IO_NR43:
              case IO_NR44:
              case IO_NR50:
              case IO_NR51:
              case IO_NR52:
              case IO_WAVE_PATTERN:
                printf("sound write to 0x%x, 0x%x\n", addr, byte);
                break;

              case IO_BGP:
                printf("BGP write 0x%x\n", byte);
                break;

              case IO_SCROLLX:
                printf("SCROLLX 0x%x\n", byte);
                break;

              case IO_SCROLLY:
                printf("SCROLLY 0x%x\n", byte);
                break;

              case IO_LCDC:
                printf("LCDC: 0x%x\n", byte);
                break;

              case IO_P1:
              case IO_SERIAL_SB:
              case IO_SERIAL_SC:
              case IO_DIV:
              case IO_TIMA:
              case IO_TMA:
              case IO_TAC:
              case IO_IF:

              case IO_STAT:

              case IO_LY:
              case IO_LYC:
              case IO_DMA:

              case IO_OBP0:
              case IO_OBP1:
              case IO_WINY:
              case IO_WINX:
                printf("unhandled I/O write @ 0x%x\n", addr);
                assert(false);
                break;
              default:
                printf("unknown I/O write @ 0x%x\n", addr);
                break;
            }
            break;
          }
        default:
          assert(false);
      }
      break;
    default:
      assert(false);
  }
}