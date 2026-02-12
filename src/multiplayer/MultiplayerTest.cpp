#include <stdio.h>

#include <nds.h>
#include <dswifi9.h>
#include <dswifi_common.h>
#include "MultiplayerTest.h"

static PrintConsole topScreen;
static PrintConsole bottomScreen;

static Wifi_AccessPoint AccessPoint;

// Global game state
// =================

#define MAX_CLIENTS 3


typedef struct {
	const char* name;
    const char* text;
} message_info;

typedef struct {
    u8 has_started;
    u8 player_mask;
    message_info message;
} pkt_host_to_client;


typedef struct {
    message_info message;
} pkt_client_to_host;

void SendHostStateToClients(void)
{
    pkt_host_to_client host_packet;

    for (int i = 0; i < WIFI_MAX_MULTIPLAYER_CLIENTS + 1; i++)
    {
		host_packet.message.name = "Host";
		host_packet.message.text = "Welcome to the game!";
    }

    Wifi_MultiplayerHostCmdTxFrame(&host_packet, sizeof(host_packet));
}

void SendClientStateToHost(int aid)
{
    pkt_client_to_host client_packet;
    client_packet.message.name = "Client";
    client_packet.message.text = "Hello, host!";
    Wifi_MultiplayerClientReplyTxFrame(&client_packet, sizeof(client_packet));
}

void FromClientPacketHandler(Wifi_MPPacketType type, int aid, int base, int len)
{
    if (len < sizeof(pkt_client_to_host))
    {
        // TODO: This shouldn't have happened!
        return;
    }

    if (type != WIFI_MPTYPE_REPLY)
        return;

    // Save information received from the client into the global state struct
    pkt_client_to_host packet;
    Wifi_RxRawReadPacket(base, sizeof(packet), (void*)&packet);

    printf("Received from AID %d: Name='%s' Text='%s'\n", aid,
		packet.message.name, packet.message.text);

}

void HostMode(void)
{
    consoleSelect(&topScreen);

    Wifi_MultiplayerHostMode(MAX_CLIENTS, sizeof(pkt_host_to_client),
        sizeof(pkt_client_to_host));



    Wifi_MultiplayerFromClientSetPacketHandler(FromClientPacketHandler);

    while (!Wifi_LibraryModeReady())
        swiWaitForVBlank();

    // You can call the next functions before loading a beacon to set up the
    // beacon. They can also be called afterwards.
    Wifi_SetChannel(6);
    Wifi_MultiplayerAllowNewClients(true);

    Wifi_BeaconStart("NintendoDS", 0xCAFEF00D);

    swiWaitForVBlank();
    swiWaitForVBlank();

    printf("Host ready!\n");

    consoleSelect(&bottomScreen);
    consoleClear();

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();

        u16 keys_down = keysDown();

        consoleClear();
        printf("RIGHT: Channel 11\n");
        printf("LEFT:  Channel 1\n");
        printf("A:     Start game\n");
        printf("\n");

        if (keys_down & KEY_RIGHT)
            Wifi_SetChannel(11);
        if (keys_down & KEY_LEFT)
            Wifi_SetChannel(1);

        if (keys_down & KEY_A)
            break;

        int num_clients = Wifi_MultiplayerGetNumClients();
        u16 players_mask = Wifi_MultiplayerGetClientMask();
        printf("Num clients: %d (mask 0x%02X)\n", num_clients, players_mask);
        printf("\n");

        // Print all client information. This normally isn't needed, all you
        // need is the mask of AIDs.
        Wifi_ConnectedClient client[15];
        num_clients = Wifi_MultiplayerGetClients(15, &(client[0]));

        for (int i = 0; i < num_clients; i++)
        {
            printf("%d (%d) %04X:%04X:%04X\n", client[i].association_id,
                client[i].state, client[i].macaddr[0], client[i].macaddr[1],
                client[i].macaddr[2]);
        }
    }

    while (1)
    {
        swiWaitForVBlank();

        SendHostStateToClients();

        consoleClear();

        printf("START: Leave host mode\n");
        printf("R:     Kick client with AID 1\n");
        printf("TOUCH: Move object\n");
        printf("\n");

        int num_clients = Wifi_MultiplayerGetNumClients();
        u16 players_mask = Wifi_MultiplayerGetClientMask();
        printf("Num clients: %d (mask 0x%02X)\n", num_clients, players_mask);
        printf("\n");


        scanKeys();

        u16 keys_down = keysDown();
        u16 keys_held = keysHeld();

        if (keys_down & KEY_START)
            break;

        if (keys_held & KEY_TOUCH)
        {
			SendHostStateToClients();
        }

        if (keys_down & KEY_R)
            Wifi_MultiplayerKickClientByAID(1);
    }

    Wifi_IdleMode();

    consoleSelect(&bottomScreen);
    consoleClear();

    consoleSelect(&topScreen);
    printf("Left host mode\n");
}

void FromHostPacketHandler(Wifi_MPPacketType type, int base, int len)
{
    if (len < sizeof(pkt_host_to_client))
    {
        // TODO: This shouldn't have happened!
        return;
    }

    if (type != WIFI_MPTYPE_CMD)
        return;

    // Save information received from the client into the global state struct
    pkt_host_to_client packet;
    Wifi_RxRawReadPacket(base, sizeof(packet), (void*)&packet);

    for (int i = 0; i < WIFI_MAX_MULTIPLAYER_CLIENTS + 1; i++)
    {
		printf("Host message: Name='%s' Text='%s'\n", packet.message.name, packet.message.text);
    }
}

bool AccessPointSelectionMenu(void)
{
    consoleSelect(&topScreen);

    Wifi_MultiplayerClientMode(sizeof(pkt_client_to_host));

    while (!Wifi_LibraryModeReady())
        swiWaitForVBlank();

    Wifi_ScanMode();

    int chosen = 0;

    consoleSelect(&bottomScreen);

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        uint16_t keys = keysDown();

        if (keys & KEY_B)
            return false;

        // Get find out how many APs there are in the area
        int count = Wifi_GetNumAP();

        consoleClear();

        printf("Number of AP: %d\n", count);
        printf("\n");

        if (count == 0)
            continue;

        if (keys & KEY_UP)
            chosen--;

        if (keys & KEY_DOWN)
            chosen++;

        if (chosen < 0)
            chosen = 0;
        if (chosen >= count)
            chosen = count - 1;

        int first = chosen - 1;
        if (first < 0)
            first = 0;

        int last = first + 3;
        if (last >= count)
            last = count - 1;

        for (int i = first; i <= last; i++)
        {
            Wifi_AccessPoint ap;
            Wifi_GetAPData(i, &ap);

            // Note that the libnds console can only print ASCII characters. If
            // the name uses characters outside of that character set, printf()
            // won't be able to print them.
            char name[10 * 4 + 1];
            int ret = utf16_to_utf8(name, sizeof(name), (char16_t*)ap.nintendo.name, ap.nintendo.name_len * 2);
            if (ret <= 0)
                name[0] = '\0';

            // In multiplayer client mode DSWiFi ignores all access points that
            // don't offer any Nintendo information. Also, DSWiFi host access
            // points don't use any encryption.

            if (ap.nintendo.allows_connections)
                consoleSetColor(&bottomScreen, CONSOLE_DEFAULT);
            else
                consoleSetColor(&bottomScreen, CONSOLE_LIGHT_RED);

            printf("%s [%.24s]\n", i == chosen ? "->" : "  ", ap.ssid);
            printf("   Name: [%.19s]\n", name);
            printf("   Players %d/%d | %08X\n", ap.nintendo.players_current,
                ap.nintendo.players_max, (unsigned int)ap.nintendo.game_id);
            printf("   %-4s | Ch %2d | RSSI %d\n",
                Wifi_ApSecurityTypeString(ap.security_type), ap.channel,
                ap.rssi);
            printf("\n");

            if (i == chosen)
                AccessPoint = ap;

            consoleSetColor(&bottomScreen, CONSOLE_DEFAULT);
        }

        if (keys & KEY_A)
            return true;
    }

    consoleClear();
}

void ClientMode(void)
{
    printf("Start client mode\n");

connect:
    if (!AccessPointSelectionMenu())
        goto end;

    consoleSelect(&topScreen);

    Wifi_MultiplayerFromHostSetPacketHandler(FromHostPacketHandler);

    printf("Selected network:\n");
    printf("\n");
    printf("%.31s\n", AccessPoint.ssid);
    printf("Channel: %d\n", AccessPoint.channel);
    printf("\n");

    Wifi_ConnectOpenAP(&AccessPoint);

    consoleSelect(&bottomScreen);
    consoleClear();

    printf("Connecting to AP\n");
    printf("Press B to cancel\n");
    printf("\n");

    // Wait until we're connected

    int oldstatus = -1;
    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        if (keysDown() & KEY_B)
            goto connect;

        int status = Wifi_AssocStatus();

        if (status != oldstatus)
        {
            printf("%s\n", ASSOCSTATUS_STRINGS[status]);
            oldstatus = status;
        }

        if (status == ASSOCSTATUS_CANNOTCONNECT)
        {
            printf("\n");
            printf("Cannot connect to AP\n");
            printf("Press START to restart\n");

            while (1)
            {
                swiWaitForVBlank();
                scanKeys();
                if (keysDown() & KEY_START)
                    goto connect;
            }
        }

        if (status == ASSOCSTATUS_ASSOCIATED)
            break;
    }

    printf("Connected to host!\n");


    consoleSelect(&bottomScreen);
    consoleClear();

    printf("Waiting for the game to start\n");
    printf("START: Disconnect\n");


    consoleSelect(&topScreen);
    printf("Game started!\n");

    consoleSelect(&bottomScreen);

    while (1)
    {
        swiWaitForVBlank();

        consoleClear();
        printf("START: Disconnect\n");
        printf("TOUCH: Move object\n");


        scanKeys();

        u16 keys_down = keysDown();
        u16 keys_held = keysHeld();

        if (keys_down & KEY_START)
            break;

        if (keys_held & KEY_TOUCH)
        {
            // Prepare new packet to be sent to the host with the current touch
            // screen coordinates.

            /*touchPosition touch_pos;
            touchRead(&touch_pos);

            pkt_client_to_host packet;
            packet.x = touch_pos.px;
            packet.y = touch_pos.py;

            Wifi_MultiplayerClientReplyTxFrame(&packet, sizeof(packet));*/

            SendClientStateToHost(0);
        }
    }

end:
    Wifi_IdleMode();

    consoleSelect(&topScreen);
    printf("Left client mode\n");
}

void startMult()
{
    printf("Initializing WiFi...\n");
    return;

    //if (!Wifi_InitDefault(INIT_ONLY | WIFI_LOCAL_ONLY))
    //{
    //    printf("Can't initialize WiFi!\n");
    //}

    //printf("WiFi initialized!\n");

    //while (1)
    //{
    //    consoleSelect(&bottomScreen);
    //    consoleClear();

    //    printf("X: Host mode\n");
    //    printf("Y: Client mode\n");

    //    while (1)
    //    {
    //        swiWaitForVBlank();
    //        scanKeys();

    //        u16 keys = keysDown();

    //        if (keys & KEY_X)
    //        {
    //            HostMode();
    //            break;
    //        }
    //        if (keys & KEY_Y)
    //        {
    //            ClientMode();
    //            break;
    //        }
    //    }
    //}

    //Wifi_DisableWifi();
}