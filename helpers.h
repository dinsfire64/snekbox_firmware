#ifndef _HELPERS_H_
#define _HELPERS_H_

#define DISPATCH_NEW_REPORT(name)    \
    case HANDLER_##name:              \
        process##name(report, len); \
        break;

#define DISPATCH_NEW_RUMBLE(name)                                         \
    case HANDLER_##name:                                                  \
        switch (runtime_settings.retro_io_mode)                         \
        {                                                                 \
        case MODE_CONSOLE_PS2:                                            \
            set##name##Rumble(current_device.dev_addr,                    \
                              current_device.instance,                    \
                              output_report.rumbleLarge,                  \
                              output_report.rumbleSmall);                 \
            break;                                                        \
                                                                          \
        case MODE_CONSOLE_GCN:                                            \
            set##name##Rumble(current_device.dev_addr,                    \
                              current_device.instance,                    \
                              output_report.rumble_active ? 0xFF : 0x00,  \
                              output_report.rumble_active ? 0xFF : 0x00); \
            break;                                                        \
                                                                          \
        default:                                                          \
            break;                                                        \
        }                                                                 \
        break;

#endif