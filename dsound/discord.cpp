#include "Discord.h"
#include "chrono"
static int64_t eptime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
void Discord::Initialize()
{
	DiscordEventHandlers Handle;
	memset(&Handle, 0, sizeof(Handle));
	Discord_Initialize("997409672229302293", &Handle, 1, NULL);
}

void Discord::Update(const char* Dstatus)
{
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	discordPresence.state = Dstatus;
	discordPresence.details = "AK 8BP 100 SAFE CHEAT/AutoPlay";
	discordPresence.startTimestamp = eptime;
	//discordPresence.endTimestamp = 1507665886;
	discordPresence.largeImageKey = "ak8bp";
	discordPresence.largeImageText = "AK 8BP CHEAT";
	discordPresence.smallImageKey = "000";
	//discordPresence.smallImageText = "Link- ";
	Discord_UpdatePresence(&discordPresence);
}