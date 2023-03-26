import requests
import json

client_id = f'4o7ew13wgsift0udz1c1xhjfh86z4j'
client_secret = f'hdzp6xpytfy9aahx6vzxl0g9cv0ey1'
access_token = f'9ffc0ewc1l1jbhueiaqdhohp6ma82p'
headers = {"Client-Id" : client_id, "Authorization" : "Bearer "+access_token}

def GetAuthAccessToken():
    req = requests.post(f'https://id.twitch.tv/oauth2/token?client_id={client_id}&client_secret={client_secret}&grant_type=client_credentials')
    print(req.text)
    return req.text

def GetClientId():
    return client_id

def GetClientSecretId():
    return client_secret

def GetUsers( userName ):
    req = requests.get(f'https://api.twitch.tv/helix/users?login={userName}', headers=headers)
    print(f'GetUsers')
    print(req.text)
    return req.text    

def GetChannelInfo( broadCasterId ):
    req = requests.get(f'https://api.twitch.tv/helix/channels?broadcaster_id={broadCasterId}', headers=headers)
    print(f'GetChannelInfo')
    print(req.text)
    return req.text    

def IsOnline( userid ):
    req = requests.get(f'https://api.twitch.tv/helix/streams?user_id={userid}', headers=headers)
    
    bOnline = False
    Online = json.loads(req.text)
    
    if( len(Online['data']) == 0 ):
        bOnline = False
    else:
        bOnline = True

    return bOnline

def GetSteam( userid ):
    req = requests.get(f'https://api.twitch.tv/helix/streams?user_id={userid}', headers=headers)
    print(f'GetSteam')
    print(req.text)
    return req.text    

def GetEmoteList( broadCasterId ):
    req = requests.get(f'https://api.twitch.tv/helix/chat/emotes?broadcaster_id={broadCasterId}', headers=headers)
    print(f'GetEmoteList')

    emoteData = json.loads(req.text)
    jsonData = emoteData['data']

    emoteList = list()

    for i in jsonData:
        emoteList.append(i['name'])

    return emoteList

GetEmoteList( 702754423 )

GetUsers(f'woowakgood')
GetSteam(702754423)
IsOnline( 7027544231 )
# {"data":[],"pagination":{}}
GetChannelInfo( 702754423 )
#GetChannelInfo( 49045679 )
#GetSteam( 49045679 )