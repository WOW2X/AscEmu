/*
 * AscEmu Framework based on ArcEmu MMORPG Server
 * Copyright (c) 2014-2017 AscEmu Team <http://www.ascemu.org/>
 * Copyright (C) 2008-2012 ArcEmu Team <http://www.ArcEmu.org/>
 * Copyright (C) 2005-2007 Ascent Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "StdAfx.h"
#include "crc32.h"
#include "Spell/SpellNameHashes.h"
#include "Units/Players/PlayerClasses.hpp"
#include "Server/MainServerDefines.h"
#include "Spell/SpellAuras.h"
#include "Server/World.h"
#include "Server/World.Legacy.h"
#include "Spell/Definitions/SpellModifierType.h"
#include "Spell/Definitions/SpellInFrontStatus.h"
#include "Spell/Definitions/SpellDamageType.h"
#include "Spell/Definitions/ProcFlags.h"
#include "Spell/Definitions/CastInterruptFlags.h"
#include <Spell/Definitions/AuraInterruptFlags.h>
#include "Spell/Definitions/SpellRanged.h"
#include "Spell/Definitions/SpellCoefficientsFlags.h"
#include "Spell/Definitions/DispelType.h"
#include "Spell/Definitions/SpellMechanics.h"
#include "Spell/Definitions/SpellEffectTarget.h"
#include "Spell/SpellHelpers.h"
#include "SpellCustomizations.hpp"

using ascemu::World::Spell::Helpers::decimalToMask;

void CreateDummySpell(uint32 id)
{
    const char* name = "Dummy Trigger";
    SpellInfo* sp = new SpellInfo;
    memset(sp, 0, sizeof(SpellInfo));
    sp->setId(id);
    sp->Attributes = 384;
    sp->AttributesEx = 268435456;
    sp->AttributesExB = 4;
    sp->CastingTimeIndex = 1;
    sp->procChance = 75;
    sp->rangeIndex = 13;
    sp->EquippedItemClass = uint32(-1);
    sp->Effect[0] = SPELL_EFFECT_DUMMY;
    sp->EffectImplicitTargetA[0] = 25;
    sp->custom_NameHash = crc32((const unsigned char*)name, (unsigned int)strlen(name));
    sp->dmg_multiplier[0] = 1.0f;
#if VERSION_STRING != Cata
    sp->StanceBarOrder = -1;
#endif
    sWorld.dummySpellList.push_back(sp);
}

void Modify_EffectBasePoints(SpellInfo* sp)
{
    if (sp == nullptr)
    {
        LOG_ERROR("Something tried to call with an invalid spell pointer!");
        return;
    }

    //Rogue: Poison time fix for 2.3
    if (sp->getId() == 3408)                 // Crippling Poison && Effect[0] == SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY
        sp->EffectBasePoints[0] = 3599;
    if (sp->getId() == 5761)                 // Mind-numbing Poison && Effect[0] == SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY
        sp->EffectBasePoints[0] = 3599;
    if (sp->getId() == 8679)                 // Instant Poison && Effect[0] == SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY
        sp->EffectBasePoints[0] = 3599;
    if (sp->getId() == 2823)                 // Deadly Poison && Effect[0] == SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY
        sp->EffectBasePoints[0] = 3599;
    if (sp->getId() == 13219)                // Wound Poison && Effect[0] == SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY
        sp->EffectBasePoints[0] = 3599;
    if (sp->getId() == 26785)                // Anesthetic Poison && Effect[0] == SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY
        sp->EffectBasePoints[0] = 3599;

    // Zyres: According to the description the weapon damage gets increased from 2 to 12 (depends on the different spell ids)
    if (sp->getId() == 2828 || sp->getId() == 29452 || sp->getId() == 29453 || sp->getId() == 56308) //Sharpen Blade && Effect[0] == SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY
        sp->EffectBasePoints[0] = 3599;

    // Set the diff. EffectBasePoint from description.
    if (sp->getId() == 11119)     // Ignite Rank 1
        sp->EffectBasePoints[0] = 8;
    if (sp->getId() == 11120)     // Ignite Rank 2
        sp->EffectBasePoints[0] = 16;
    if (sp->getId() == 12846)     // Ignite Rank 3
        sp->EffectBasePoints[0] = 24;
    if (sp->getId() == 12847)     // Ignite Rank 4
        sp->EffectBasePoints[0] = 32;
    if (sp->getId() == 12848)     // Ignite Rank 5
        sp->EffectBasePoints[0] = 40;
}

void Set_missing_spellLevel(SpellInfo* sp)
{
    if (sp == nullptr)
    {
        LOG_ERROR("Something tried to call with an invalid spell pointer!");
        return;
    }

    //stupid spell ranking problem
    if (sp->spellLevel == 0)
    {
        uint32 new_level = 0;

        // 16/03/08 Zyres: just replaced name assignes with spell ids. \todo remove not teachable spells.
        switch (sp->getId())
        {
            // name "Aprentice "
            case 2020:
            case 2155:
            case 2275:
            case 2372:
            case 2551:
            case 2581:
            case 3279:
            case 3911:
            case 4039:
            case 7414:
            case 7733:
            case 8615:
            case 25245:
            case 29535:
            case 33388:
            case 33389:
            case 45375:
            case 51216:
            {
                new_level = 1;
            } break;
            // name "Journeyman "
            case 2021:
            case 2154:
            case 2280:
            case 2373:
            case 2582:
            case 3280:
            case 3412:
            case 3912:
            case 4040:
            case 7415:
            case 7734:
            case 8619:
            case 25246:
            case 33391:
            case 33392:
            case 45376:
            case 64485:
            {
                new_level = 2;
            } break;
            // name "Expert "
            case 2552:
            case 3465:
            case 3539:
            case 3568:
            case 3571:
            case 3812:
            case 3913:
            case 4041:
            case 7416:
            case 7736:
            case 7925:
            case 8620:
            case 19886:
            case 19889:
            case 19903:
            case 28896:
            case 34090:
            case 34092:
            case 45377:
            case 54083:
            case 54254:
            case 54257:
            {
                new_level = 3;
            } break;
            // name "Artisan "
            case 9786:
            case 10249:
            case 10663:
            case 10769:
            case 10847:
            case 11612:
            case 11994:
            case 12181:
            case 12657:
            case 13921:
            case 18249:
            case 18261:
            case 19887:
            case 19890:
            case 19902:
            case 28899:
            case 34091:
            case 34093:
            case 45378:
            {
                new_level = 4;
            } break;
            // name "Master "
            case 12613:
            case 13958:
            case 13970:
            case 13971:
            case 14904:
            case 15024:
            case 15025:
            case 15026:
            case 15027:
            case 17039:
            case 17040:
            case 17041:
            case 18709:
            case 18710:
            case 18767:
            case 18768:
            case 19825:
            case 21935:
            case 21940:
            case 23759:
            case 23760:
            case 23761:
            case 23762:
            case 23784:
            case 23785:
            case 23822:
            case 23823:
            case 23824:
            case 23825:
            case 23826:
            case 23827:
            case 23828:
            case 23829:
            case 23830:
            case 23831:
            case 23832:
            case 23833:
            case 23834:
            case 23835:
            case 23836:
            case 23837:
            case 23838:
            case 23839:
            case 23840:
            case 23841:
            case 23842:
            case 23843:
            case 23844:
            case 24347:
            case 24626:
            case 24627:
            case 26791:
            case 27029:
            case 27235:
            case 27236:
            case 27237:
            case 28030:
            case 28597:
            case 28696:
            case 28901:
            case 29074:
            case 29075:
            case 29076:
            case 29077:
            case 29355:
            case 29460:
            case 29461:
            case 29845:
            case 30351:
            case 31221:
            case 31222:
            case 31223:
            case 31226:
            case 31227:
            case 31665:
            case 31666:
            case 32550:
            case 32679:
            case 33098:
            case 33100:
            case 33360:
            case 33361:
            case 33894:
            case 34130:
            case 34149:
            case 34150:
            case 34485:
            case 34486:
            case 34487:
            case 34488:
            case 34489:
            case 34506:
            case 34507:
            case 34508:
            case 34833:
            case 34834:
            case 34835:
            case 34836:
            case 34837:
            case 34838:
            case 34839:
            case 35702:
            case 35703:
            case 35704:
            case 35705:
            case 35706:
            case 35708:
            case 35874:
            case 35912:
            case 36001:
            case 38734:
            case 39097:
            case 39098:
            case 39099:
            case 40369:
            case 40385:
            case 40388:
            case 43784:
            case 44868:
            case 45379:
            case 45713:
            case 46417:
            case 46418:
            case 46444:
            case 47872:
            case 47873:
            case 47874:
            case 47875:
            case 47876:
            case 47877:
            case 48028:
            case 48411:
            case 48412:
            case 48418:
            case 48420:
            case 48421:
            case 48422:
            case 48729:
            case 48730:
            case 49776:
            case 52143:
            case 52559:
            case 53125:
            case 53662:
            case 53663:
            case 53664:
            case 53665:
            case 53666:
            case 54084:
            case 54255:
            case 54256:
            case 54721:
            case 55188:
            case 55434:
            case 55435:
            case 57853:
            case 58410:
            case 62698:
            case 65022:
            case 66409:
            case 66410:
            case 66417:
            case 66420:
            case 66423:
            case 66457:
            case 66460:
            case 66461:
            case 66677:
            case 66810:
            case 66812:
            case 67551:
            case 69716:
            case 70528:
            {
                new_level = 5;
            } break;
            // name "Grand Master "
            case 45380:
            case 50299:
            case 50301:
            case 50307:
            case 50309:
            case 51293:
            case 51295:
            case 51298:
            case 51301:
            case 51303:
            case 51305:
            case 51308:
            case 51310:
            case 51312:
            case 61464:
            case 64484:
            case 65281:
            case 65282:
            case 65283:
            case 65284:
            case 65285:
            case 65286:
            case 65287:
            case 65288:
            case 65289:
            case 65290:
            case 65291:
            case 65292:
            case 65293:
            {
                new_level = 6;
            } break;
            default:
                break;
        }

        if (new_level != 0)
        {
            uint32 teachspell = 0;
            if (sp->Effect[0] == SPELL_EFFECT_LEARN_SPELL)
                teachspell = sp->EffectTriggerSpell[0];
            else if (sp->Effect[1] == SPELL_EFFECT_LEARN_SPELL)
                teachspell = sp->EffectTriggerSpell[1];
            else if (sp->Effect[2] == SPELL_EFFECT_LEARN_SPELL)
                teachspell = sp->EffectTriggerSpell[2];

            if (teachspell)
            {
                SpellInfo* spellInfo = Spell::checkAndReturnSpellEntry(teachspell);
                if (spellInfo != nullptr)
                {
                    spellInfo->spellLevel = new_level;
                    sp->spellLevel = new_level;
                }
            }
        }
    }
}

void Modify_AuraInterruptFlags(SpellInfo* sp)
{
    if (sp == nullptr)
    {
        LOG_ERROR("Something tried to call with an invalid spell pointer!");
        return;
    }

    // HACK FIX: Break roots/fear on damage.. this needs to be fixed properly!
    if (!(sp->AuraInterruptFlags & AURA_INTERRUPT_ON_ANY_DAMAGE_TAKEN))
    {
        for (uint32 z = 0; z < 3; ++z)
        {
            if (sp->EffectApplyAuraName[z] == SPELL_AURA_MOD_FEAR || sp->EffectApplyAuraName[z] == SPELL_AURA_MOD_ROOT)
            {
                sp->AuraInterruptFlags |= AURA_INTERRUPT_ON_UNUSED2;
                break;
            }
        }
    }
}

void Modify_RecoveryTime(SpellInfo* sp)
{
    if (sp == nullptr)
    {
        LOG_ERROR("Something tried to call with an invalid spell pointer!");
        return;
    }

    // Description includes
    switch (sp->getId())
    {
        // "Must remain seated" 154 rows o.O
        case 430:
        case 431:
        case 432:
        case 433:
        case 434:
        case 435:
        case 833:
        case 1127:
        case 1129:
        case 1131:
        case 1133:
        case 1135:
        case 1137:
        case 2639:
        case 5004:
        case 5005:
        case 5006:
        case 5007:
        case 7737:
        case 9177:
        case 10250:
        case 10256:
        case 10257:
        case 18071:
        case 18124:
        case 18140:
        case 18229:
        case 18230:
        case 18231:
        case 18232:
        case 18233:
        case 18234:
        case 21149:
        case 22731:
        case 22734:
        case 23540:
        case 23541:
        case 23542:
        case 23692:
        case 23698:
        case 24005:
        case 24355:
        case 24384:
        case 24409:
        case 24410:
        case 24411:
        case 24707:
        case 24800:
        case 24869:
        case 25660:
        case 25690:
        case 25691:
        case 25692:
        case 25693:
        case 25697:
        case 25700:
        case 25701:
        case 25886:
        case 25887:
        case 25990:
        case 26030:
        case 26263:
        case 27089:
        case 27094:
        case 28616:
        case 29007:
        case 29008:
        case 29029:
        case 29055:
        case 29073:
        case 30024:
        case 32112:
        case 33253:
        case 33255:
        case 33258:
        case 33260:
        case 33262:
        case 33264:
        case 33266:
        case 33269:
        case 33725:
        case 33772:
        case 34291:
        case 35270:
        case 35271:
        case 40543:
        case 40745:
        case 40768:
        case 41030:
        case 41031:
        case 42207:
        case 42308:
        case 42309:
        case 42311:
        case 42312:
        case 43154:
        case 43180:
        case 43182:
        case 43183:
        case 43706:
        case 43763:
        case 44107:
        case 44109:
        case 44110:
        case 44111:
        case 44112:
        case 44113:
        case 44114:
        case 44115:
        case 44116:
        case 44166:
        case 45019:
        case 45020:
        case 45548:
        case 45618:
        case 46683:
        case 46755:
        case 46812:
        case 46898:
        case 49472:
        case 52911:
        case 53283:
        case 53373:
        case 56439:
        case 57069:
        case 57070:
        case 57073:
        case 57084:
        case 57649:
        case 58645:
        case 58648:
        case 58886:
        case 61827:
        case 61828:
        case 61829:
        case 61830:
        case 61874:
        case 64056:
        case 64354:
        case 64355:
        case 64356:
        case 65363:
        case 65418:
        case 65419:
        case 65420:
        case 65421:
        case 65422:
        case 69560:
        case 69561:
        case 71068:
        case 71071:
        case 71073:
        case 71074:
        case 72623:
        {
            sp->RecoveryTime = 1000;
            sp->CategoryRecoveryTime = 1000;
        } break;
        default:
            break;
    }
}

void ApplyObsoleteNameHash(SpellInfo* sp)
{
    switch (sp->getId())
    {
        case 58984:
        case 62196:
        case 62199:
        {
            sp->custom_NameHash = SPELL_HASH_SHADOWMELD;
            break;
        }
        case 3043:
        case 18545:
        case 52604:
        {
            sp->custom_NameHash = SPELL_HASH_SCORPID_STING;
            break;
        }
        case 19386:
        case 24131:
        case 24132:
        case 24133:
        case 24134:
        case 24135:
        case 24335:
        case 24336:
        case 26180:
        case 26233:
        case 26748:
        case 27068:
        case 27069:
        case 41186:
        case 49009:
        case 49010:
        case 49011:
        case 49012:
        case 65877:
        case 65878:
        {
            sp->custom_NameHash = SPELL_HASH_WYVERN_STING;
            break;
        }
        case 1978:
        case 13549:
        case 13550:
        case 13551:
        case 13552:
        case 13553:
        case 13554:
        case 13555:
        case 25295:
        case 27016:
        case 31975:
        case 35511:
        case 36984:
        case 38859:
        case 38914:
        case 39182:
        case 49000:
        case 49001:
        {
            sp->custom_NameHash = SPELL_HASH_SERPENT_STING;
            break;
        }
        case 3034:
        case 31407:
        case 37551:
        case 39413:
        case 65881:
        case 67991:
        case 67992:
        case 67993:
        {
            sp->custom_NameHash = SPELL_HASH_VIPER_STING;
            break;
        }
        case 54197:
        {
            sp->custom_NameHash = SPELL_HASH_COLD_WEATHER_FLYING;
            break;
        }
        case 6143:
        case 6144:
        case 8461:
        case 8462:
        case 8463:
        case 8464:
        case 10177:
        case 10178:
        case 15044:
        case 25641:
        case 27396:
        case 28609:
        case 32796:
        case 32797:
        case 43012:
        {
            sp->custom_NameHash = SPELL_HASH_FROST_WARD;
            break;
        }
        case 543:
        case 1035:
        case 8457:
        case 8458:
        case 8459:
        case 8460:
        case 10223:
        case 10224:
        case 10225:
        case 10226:
        case 15041:
        case 27128:
        case 27395:
        case 37844:
        case 43010:
        {
            sp->custom_NameHash = SPELL_HASH_FIRE_WARD;
            break;
        }
        case 20185:
        case 20267:
        case 20271:
        case 28775:
        case 57774:
        {
            sp->custom_NameHash = SPELL_HASH_JUDGEMENT_OF_LIGHT;
            break;
        }
        case 20186:
        case 20268:
        case 53408:
        {
            sp->custom_NameHash = SPELL_HASH_JUDGEMENT_OF_WISDOM;
            break;
        }
        case 20184:
        case 53407:
        {
            sp->custom_NameHash = SPELL_HASH_JUDGEMENT_OF_JUSTICE;
            break;
        }
        case 31804:
        {
            sp->custom_NameHash = SPELL_HASH_JUDGEMENT_OF_VENGEANCE;
            break;
        }
        case 53733:
        {
            sp->custom_NameHash = SPELL_HASH_JUDGEMENT_OF_CORRUPTION;
            break;
        }
        case 20187:
        {
            sp->custom_NameHash = SPELL_HASH_JUDGEMENT_OF_RIGHTEOUSNESS;
            break;
        }
        case 589:
        case 594:
        case 970:
        case 992:
        case 2767:
        case 10892:
        case 10893:
        case 10894:
        case 11639:
        case 14032:
        case 15654:
        case 17146:
        case 19776:
        case 23268:
        case 23952:
        case 24212:
        case 25367:
        case 25368:
        case 27605:
        case 30854:
        case 30898:
        case 34441:
        case 34941:
        case 34942:
        case 37275:
        case 41355:
        case 46560:
        case 48124:
        case 48125:
        case 57778:
        case 59864:
        case 60005:
        case 60446:
        case 65541:
        case 68088:
        case 68089:
        case 68090:
        case 72318:
        case 72319:
        {
            sp->custom_NameHash = SPELL_HASH_SHADOW_WORD__PAIN;
            break;
        }
        case 44394:
        case 44395:
        case 44396:
        case 44413:
        {
            sp->custom_NameHash = SPELL_HASH_INCANTER_S_ABSORPTION;
            break;
        }
        case 4:
        {
            sp->custom_NameHash = SPELL_HASH_WORD_OF_RECALL_OTHER;
            break;
        }
        case 32546:
        case 48119:
        case 48120:
        {
            sp->custom_NameHash = SPELL_HASH_BINDING_HEAL;
            break;
        }
        case 2061:
        case 9472:
        case 9473:
        case 9474:
        case 10915:
        case 10916:
        case 10917:
        case 17137:
        case 17138:
        case 17843:
        case 25233:
        case 25235:
        case 27608:
        case 38588:
        case 42420:
        case 43431:
        case 43516:
        case 43575:
        case 48070:
        case 48071:
        case 56331:
        case 56919:
        case 66104:
        case 68023:
        case 68024:
        case 68025:
        case 71595:
        case 71782:
        case 71783:
        {
            sp->custom_NameHash = SPELL_HASH_FLASH_HEAL;
            break;
        }
        case 10833:
        case 16067:
        case 18091:
        case 20883:
        case 22893:
        case 22920:
        case 22940:
        case 24857:
        case 30451:
        case 30661:
        case 31457:
        case 32935:
        case 34793:
        case 35314:
        case 35927:
        case 36032:
        case 37126:
        case 38342:
        case 38344:
        case 38538:
        case 38881:
        case 40837:
        case 40881:
        case 42894:
        case 42896:
        case 42897:
        case 49198:
        case 50545:
        case 51797:
        case 51830:
        case 56969:
        case 58462:
        case 59257:
        case 59909:
        case 65791:
        case 67997:
        case 67998:
        case 67999:
        {
            sp->custom_NameHash = SPELL_HASH_ARCANE_BLAST;
            break;
        }
        case 2139:
        case 15122:
        case 19715:
        case 20537:
        case 20788:
        case 29443:
        case 29961:
        case 31596:
        case 31999:
        case 37470:
        case 51610:
        case 65790:
        {
            sp->custom_NameHash = SPELL_HASH_COUNTERSPELL;
            break;
        }
        case 3252:
        case 5221:
        case 6800:
        case 8992:
        case 9829:
        case 9830:
        case 27001:
        case 27002:
        case 27555:
        case 48571:
        case 48572:
        case 49121:
        case 49165:
        case 61548:
        case 61549:
        {
            sp->custom_NameHash = SPELL_HASH_SHRED;
            break;
        }
        case 6807:
        case 6808:
        case 6809:
        case 7092:
        case 8972:
        case 9745:
        case 9880:
        case 9881:
        case 12161:
        case 15793:
        case 17156:
        case 20751:
        case 26996:
        case 27553:
        case 34298:
        case 48479:
        case 48480:
        case 51875:
        case 52506:
        case 54459:
        {
            sp->custom_NameHash = SPELL_HASH_MAUL;
            break;
        }
        case 19820:
        case 22689:
        case 31041:
        case 33917:
        case 39164:
        case 41439:
        case 42389:
        case 57657:
        case 59988:
        case 71925:
        {
            sp->custom_NameHash = SPELL_HASH_MANGLE;
            break;
        }
        case 563:
        case 8921:
        case 8924:
        case 8925:
        case 8926:
        case 8927:
        case 8928:
        case 8929:
        case 9833:
        case 9834:
        case 9835:
        case 15798:
        case 20690:
        case 21669:
        case 22206:
        case 23380:
        case 24957:
        case 26987:
        case 26988:
        case 27737:
        case 31270:
        case 31401:
        case 32373:
        case 32415:
        case 37328:
        case 43545:
        case 45821:
        case 45900:
        case 47072:
        case 48462:
        case 48463:
        case 52502:
        case 57647:
        case 59987:
        case 65856:
        case 67944:
        case 67945:
        case 67946:
        case 75329:
        {
            sp->custom_NameHash = SPELL_HASH_MOONFIRE;
            break;
        }
        case 33876:
        case 33982:
        case 33983:
        case 48565:
        case 48566:
        {
            sp->custom_NameHash = SPELL_HASH_MANGLE__CAT_;
            break;
        }
        case 33878:
        case 33986:
        case 33987:
        case 48563:
        case 48564:
        {
            sp->custom_NameHash = SPELL_HASH_MANGLE__BEAR_;
            break;
        }
        case 45477:
        case 49723:
        case 49896:
        case 49903:
        case 49904:
        case 49909:
        case 50349:
        case 52372:
        case 52378:
        case 53549:
        case 55313:
        case 55331:
        case 59011:
        case 59131:
        case 60952:
        case 66021:
        case 67718:
        case 67881:
        case 67938:
        case 67939:
        case 67940:
        case 69916:
        case 70589:
        case 70591:
        {
            sp->custom_NameHash = SPELL_HASH_ICY_TOUCH;
            break;
        }
        case 45902:
        case 49926:
        case 49927:
        case 49928:
        case 49929:
        case 49930:
        case 52374:
        case 52377:
        case 59130:
        case 60945:
        case 61696:
        case 66215:
        case 66975:
        case 66976:
        case 66977:
        case 66978:
        case 66979:
        {
            sp->custom_NameHash = SPELL_HASH_BLOOD_STRIKE;
            break;
        }
        case 55050:
        case 55258:
        case 55259:
        case 55260:
        case 55261:
        case 55262:
        case 55978:
        {
            sp->custom_NameHash = SPELL_HASH_HEART_STRIKE;
            break;
        }
        case 45463:
        case 45469:
        case 45470:
        case 49923:
        case 49924:
        case 49998:
        case 49999:
        case 53639:
        case 66188:
        case 66950:
        case 66951:
        case 66952:
        case 66953:
        case 71489:
        {
            sp->custom_NameHash = SPELL_HASH_DEATH_STRIKE;
            break;
        }
        case 43568:
        case 49143:
        case 51416:
        case 51417:
        case 51418:
        case 51419:
        case 55268:
        case 60951:
        case 66047:
        case 66196:
        case 66958:
        case 66959:
        case 66960:
        case 66961:
        case 66962:
        case 67935:
        case 67936:
        case 67937:
        {
            sp->custom_NameHash = SPELL_HASH_FROST_STRIKE;
            break;
        }
        case 6789:
        case 17925:
        case 17926:
        case 27223:
        case 28412:
        case 30500:
        case 30741:
        case 32709:
        case 33130:
        case 34437:
        case 35954:
        case 38065:
        case 39661:
        case 41070:
        case 44142:
        case 46283:
        case 47541:
        case 47632:
        case 47633:
        case 47859:
        case 47860:
        case 49892:
        case 49893:
        case 49894:
        case 49895:
        case 50668:
        case 52375:
        case 52376:
        case 53769:
        case 55209:
        case 55210:
        case 55320:
        case 56362:
        case 59134:
        case 60949:
        case 62900:
        case 62901:
        case 62902:
        case 62903:
        case 62904:
        case 65820:
        case 66019:
        case 67929:
        case 67930:
        case 67931:
        case 68139:
        case 68140:
        case 68141:
        case 71490:
        {
            sp->custom_NameHash = SPELL_HASH_DEATH_COIL;
            break;
        }
        case 10321:
        case 23590:
        case 23591:
        case 35170:
        case 41467:
        case 43838:
        case 54158:
        {
            sp->custom_NameHash = SPELL_HASH_JUDGEMENT;
            break;
        }
        case 635:
        case 639:
        case 647:
        case 1026:
        case 1042:
        case 3472:
        case 10328:
        case 10329:
        case 13952:
        case 15493:
        case 25263:
        case 25292:
        case 27135:
        case 27136:
        case 29383:
        case 29427:
        case 29562:
        case 31713:
        case 32769:
        case 37979:
        case 43451:
        case 44479:
        case 46029:
        case 48781:
        case 48782:
        case 52444:
        case 56539:
        case 58053:
        case 66112:
        case 68011:
        case 68012:
        case 68013:
        {
            sp->custom_NameHash = SPELL_HASH_HOLY_LIGHT;
            break;
        }
        case 29964:
        case 29965:
        case 31661:
        case 33041:
        case 33042:
        case 33043:
        case 35250:
        case 37289:
        case 42949:
        case 42950:
        {
            sp->custom_NameHash = SPELL_HASH_DRAGON_S_BREATH;
            break;
        }
        case 1831:
        case 11113:
        case 13018:
        case 13019:
        case 13020:
        case 13021:
        case 15091:
        case 15744:
        case 16046:
        case 17145:
        case 17277:
        case 20229:
        case 22424:
        case 23039:
        case 23113:
        case 23331:
        case 25049:
        case 27133:
        case 30092:
        case 30600:
        case 33061:
        case 33933:
        case 36278:
        case 38064:
        case 38536:
        case 38712:
        case 39001:
        case 39038:
        case 42944:
        case 42945:
        case 58970:
        case 60290:
        case 61362:
        case 66044:
        case 68312:
        case 70407:
        case 71151:
        {
            sp->custom_NameHash = SPELL_HASH_BLAST_WAVE;
            break;
        }
        case 49018:
        case 49529:
        case 49530:
        {
            sp->custom_NameHash = SPELL_HASH_SUDDEN_DOOM;
            break;
        }
        case 29166:
        {
            sp->custom_NameHash = SPELL_HASH_INNERVATE;
            break;
        }
        case 100:
        case 6178:
        case 7370:
        case 11578:
        case 20508:
        case 22120:
        case 22911:
        case 24023:
        case 24193:
        case 24315:
        case 24408:
        case 25821:
        case 25999:
        case 26184:
        case 26185:
        case 26186:
        case 26202:
        case 28343:
        case 29320:
        case 29847:
        case 31426:
        case 31733:
        case 32323:
        case 33709:
        case 34846:
        case 35412:
        case 35570:
        case 35754:
        case 36058:
        case 36140:
        case 36509:
        case 37511:
        case 38461:
        case 39574:
        case 40602:
        case 41581:
        case 42003:
        case 43519:
        case 43651:
        case 43807:
        case 44357:
        case 44884:
        case 49758:
        case 50582:
        case 51492:
        case 51756:
        case 51842:
        case 52538:
        case 52577:
        case 52856:
        case 53148:
        case 54460:
        case 55317:
        case 55530:
        case 57627:
        case 58619:
        case 58991:
        case 59040:
        case 59611:
        case 60067:
        case 61685:
        case 62563:
        case 62613:
        case 62614:
        case 62874:
        case 62960:
        case 62961:
        case 62977:
        case 63003:
        case 63010:
        case 63661:
        case 63665:
        case 64591:
        case 64719:
        case 65927:
        case 66481:
        case 68282:
        case 68284:
        case 68301:
        case 68307:
        case 68321:
        case 68498:
        case 68501:
        case 68762:
        case 68763:
        case 68764:
        case 71553:
        case 74399:
        {
            sp->custom_NameHash = SPELL_HASH_CHARGE;
            break;
        }
        case 10400:
        case 15567:
        case 15568:
        case 15569:
        case 16311:
        case 16312:
        case 16313:
        case 58784:
        case 58791:
        case 58792:
        {
            sp->custom_NameHash = SPELL_HASH_FLAMETONGUE_WEAPON__PASSIVE_;
            break;
        }
        case 5171:
        case 6434:
        case 6774:
        case 30470:
        case 43547:
        case 60847:
        {
            sp->custom_NameHash = SPELL_HASH_SLICE_AND_DICE;
            break;
        }
        case 15407:
        case 16568:
        case 17165:
        case 17311:
        case 17312:
        case 17313:
        case 17314:
        case 18807:
        case 22919:
        case 23953:
        case 25387:
        case 26044:
        case 26143:
        case 28310:
        case 29407:
        case 29570:
        case 32417:
        case 35507:
        case 37276:
        case 37330:
        case 37621:
        case 38243:
        case 40842:
        case 42396:
        case 43512:
        case 46562:
        case 48155:
        case 48156:
        case 52586:
        case 54339:
        case 54805:
        case 57779:
        case 57941:
        case 58381:
        case 59367:
        case 59974:
        case 60006:
        case 60472:
        case 65488:
        case 68042:
        case 68043:
        case 68044:
        {
            sp->custom_NameHash = SPELL_HASH_MIND_FLAY;
            break;
        }
        case 8092:
        case 8102:
        case 8103:
        case 8104:
        case 8105:
        case 8106:
        case 10945:
        case 10946:
        case 10947:
        case 13860:
        case 15587:
        case 17194:
        case 17287:
        case 20830:
        case 25372:
        case 25375:
        case 26048:
        case 31516:
        case 37531:
        case 38259:
        case 41374:
        case 48126:
        case 48127:
        case 52722:
        case 58850:
        case 60447:
        case 60453:
        case 60500:
        case 65492:
        case 68038:
        case 68039:
        case 68040:
        {
            sp->custom_NameHash = SPELL_HASH_MIND_BLAST;
            break;
        }
        case 32379:
        case 32409:
        case 32996:
        case 41375:
        case 47697:
        case 48157:
        case 48158:
        case 51818:
        case 56920:
        {
            sp->custom_NameHash = SPELL_HASH_SHADOW_WORD__DEATH;
            break;
        }
        case 34914:
        case 34916:
        case 34917:
        case 34919:
        case 48159:
        case 48160:
        case 52723:
        case 52724:
        case 60501:
        case 64085:
        case 65490:
        case 68091:
        case 68092:
        case 68093:
        {
            sp->custom_NameHash = SPELL_HASH_VAMPIRIC_TOUCH;
            break;
        }
        case 53742:
        {
            sp->custom_NameHash = SPELL_HASH_BLOOD_CORRUPTION;
            break;
        }
        case 31803:
        {
            sp->custom_NameHash = SPELL_HASH_HOLY_VENGEANCE;
            break;
        }
        case 8034:
        case 8037:
        case 10458:
        case 16352:
        case 16353:
        case 25501:
        case 38617:
        case 54609:
        case 58797:
        case 58798:
        case 58799:
        case 64186:
        {
            sp->custom_NameHash = SPELL_HASH_FROSTBRAND_ATTACK;
            break;
        }

        case 3408:
        case 3409:
        case 25809:
        case 30981:
        case 44289:
        {
            sp->custom_NameHash = SPELL_HASH_CRIPPLING_POISON;
            break;
        }

        case 5760:
        case 5761:
        case 25810:
        case 34615:
        case 41190:
        {
            sp->custom_NameHash = SPELL_HASH_MIND_NUMBING_POISON;
            break;
        }

        case 51664:
        case 51665:
        case 51667:
        case 51668:
        case 51669:
        {
            sp->custom_NameHash = SPELL_HASH_CUT_TO_THE_CHASE;
            break;
        }

        case 51625:
        case 51626:
        {
            sp->custom_NameHash = SPELL_HASH_DEADLY_BREW;
            break;
        }

        case 51692:
        case 51693:
        case 51696:
        {
            sp->custom_NameHash = SPELL_HASH_WAYLAY;
            break;
        }

        case 15337:
        case 15338:
        case 49694:
        case 59000:
        {
            sp->custom_NameHash = SPELL_HASH_IMPROVED_SPIRIT_TAP;
            break;
        }

        case 34753:
        case 34754:
        case 34859:
        case 34860:
        case 63724:
        case 63725:
        {
            sp->custom_NameHash = SPELL_HASH_HOLY_CONCENTRATION;
            break;
        }

        case 47509:
        case 47511:
        case 47515:
        case 47753:
        case 54704:
        {
            sp->custom_NameHash = SPELL_HASH_DIVINE_AEGIS;
            break;
        }

        case 63625:
        case 63626:
        case 63627:
        case 63675:
        case 75999:
        {
            sp->custom_NameHash = SPELL_HASH_IMPROVED_DEVOURING_PLAGUE;
            break;
        }

        case 15286:
        case 15290:
        case 71269:
        {
            sp->custom_NameHash = SPELL_HASH_VAMPIRIC_EMBRACE;
            break;
        }

        case 63534:
        case 63542:
        case 63543:
        case 63544:
        {
            sp->custom_NameHash = SPELL_HASH_EMPOWERED_RENEW;
            break;
        }

        case 33191:
        case 33192:
        case 33193:
        case 33196:
        case 33197:
        case 33198:
        {
            sp->custom_NameHash = SPELL_HASH_MISERY;
            break;
        }

        case 33076:
        case 33110:
        case 41635:
        case 41637:
        case 44583:
        case 44586:
        case 46045:
        case 48110:
        case 48111:
        case 48112:
        case 48113:
        {
            sp->custom_NameHash = SPELL_HASH_PRAYER_OF_MENDING;
            break;
        }

        case 15270:
        case 15271:
        case 15335:
        case 15336:
        {
            sp->custom_NameHash = SPELL_HASH_SPIRIT_TAP;
            break;
        }

        case 20375:
        case 20424:
        case 29385:
        case 33127:
        case 41469:
        case 42058:
        case 57769:
        case 57770:
        case 66004:
        case 68020:
        case 68021:
        case 68022:
        case 69403:
        {
            sp->custom_NameHash = SPELL_HASH_SEAL_OF_COMMAND;
            break;
        }

        case 9799:
        case 25988:
        case 25997:
        {
            sp->custom_NameHash = SPELL_HASH_EYE_FOR_AN_EYE;
            break;
        }

        case 43742:
        {
            sp->custom_NameHash = SPELL_HASH_GRACE_OF_THE_NAARU;
            break;
        }

        case 31785:
        case 31786:
        case 33776:
        {
            sp->custom_NameHash = SPELL_HASH_SPIRITUAL_ATTUNEMENT;
            break;
        }

        case 49182:
        case 49500:
        case 49501:
        case 51789:
        case 55225:
        case 55226:
        case 64855:
        case 64856:
        case 64858:
        case 64859:
        {
            sp->custom_NameHash = SPELL_HASH_BLADE_BARRIER;
            break;
        }

        case 41434:
        case 41435:
        {
            sp->custom_NameHash = SPELL_HASH_THE_TWIN_BLADES_OF_AZZINOTH;
            break;
        }

        case 8024:
        case 8027:
        case 8030:
        case 16339:
        case 16341:
        case 16342:
        case 25489:
        case 58785:
        case 58789:
        case 58790:
        case 65979:
        {
            sp->custom_NameHash = SPELL_HASH_FLAMETONGUE_WEAPON;
            break;
        }

        case 10:
        case 1196:
        case 6141:
        case 6142:
        case 8364:
        case 8427:
        case 8428:
        case 10185:
        case 10186:
        case 10187:
        case 10188:
        case 10189:
        case 10190:
        case 15783:
        case 19099:
        case 20680:
        case 21096:
        case 21367:
        case 25019:
        case 26607:
        case 27085:
        case 27384:
        case 27618:
        case 29458:
        case 29951:
        case 30093:
        case 31266:
        case 31581:
        case 33418:
        case 33624:
        case 33634:
        case 34167:
        case 34183:
        case 34356:
        case 37263:
        case 37671:
        case 38646:
        case 39416:
        case 41382:
        case 41482:
        case 42198:
        case 42208:
        case 42209:
        case 42210:
        case 42211:
        case 42212:
        case 42213:
        case 42937:
        case 42938:
        case 42939:
        case 42940:
        case 44178:
        case 46195:
        case 47727:
        case 49034:
        case 50715:
        case 56936:
        case 58693:
        case 59278:
        case 59369:
        case 59854:
        case 61085:
        case 62576:
        case 62577:
        case 62602:
        case 62603:
        case 62706:
        case 64642:
        case 64653:
        case 70362:
        case 70421:
        case 71118:
        {
            sp->custom_NameHash = SPELL_HASH_BLIZZARD;
            break;
        }

        case 17:
        case 592:
        case 600:
        case 3747:
        case 6065:
        case 6066:
        case 10898:
        case 10899:
        case 10900:
        case 10901:
        case 11647:
        case 11835:
        case 11974:
        case 17139:
        case 20697:
        case 22187:
        case 25217:
        case 25218:
        case 27607:
        case 29408:
        case 32595:
        case 35944:
        case 36052:
        case 41373:
        case 44175:
        case 44291:
        case 46193:
        case 48065:
        case 48066:
        case 66099:
        case 68032:
        case 68033:
        case 68034:
        case 71548:
        case 71780:
        case 71781:
        {
            sp->custom_NameHash = SPELL_HASH_POWER_WORD__SHIELD;
            break;
        }

        case 67:
        case 9452:
        case 26016:
        case 26017:
        case 36002:
        {
            sp->custom_NameHash = SPELL_HASH_VINDICATION;
            break;
        }

        case 116:
        case 205:
        case 837:
        case 7322:
        case 8406:
        case 8407:
        case 8408:
        case 9672:
        case 10179:
        case 10180:
        case 10181:
        case 11538:
        case 12675:
        case 12737:
        case 13322:
        case 13439:
        case 15043:
        case 15497:
        case 15530:
        case 16249:
        case 16799:
        case 17503:
        case 20297:
        case 20792:
        case 20806:
        case 20819:
        case 20822:
        case 21369:
        case 23102:
        case 23412:
        case 24942:
        case 25304:
        case 27071:
        case 27072:
        case 28478:
        case 28479:
        case 29457:
        case 29926:
        case 29954:
        case 30942:
        case 31296:
        case 31622:
        case 32364:
        case 32370:
        case 32984:
        case 34347:
        case 35316:
        case 36279:
        case 36710:
        case 36990:
        case 37930:
        case 38238:
        case 38534:
        case 38645:
        case 38697:
        case 38826:
        case 39064:
        case 40429:
        case 40430:
        case 41384:
        case 41486:
        case 42719:
        case 42803:
        case 42841:
        case 42842:
        case 43083:
        case 43428:
        case 44606:
        case 44843:
        case 46035:
        case 46987:
        case 49037:
        case 50378:
        case 50721:
        case 54791:
        case 55802:
        case 55807:
        case 56775:
        case 56837:
        case 57665:
        case 57825:
        case 58457:
        case 58535:
        case 59017:
        case 59251:
        case 59280:
        case 59638:
        case 59855:
        case 61087:
        case 61461:
        case 61590:
        case 61730:
        case 61747:
        case 62583:
        case 62601:
        case 63913:
        case 65807:
        case 68003:
        case 68004:
        case 68005:
        case 69274:
        case 69573:
        case 70277:
        case 70327:
        case 71318:
        case 71420:
        case 72007:
        case 72166:
        case 72167:
        case 72501:
        case 72502:
        {
            sp->custom_NameHash = SPELL_HASH_FROSTBOLT;
            break;
        }

        case 133:
        case 143:
        case 145:
        case 3140:
        case 8400:
        case 8401:
        case 8402:
        case 9053:
        case 9487:
        case 9488:
        case 10148:
        case 10149:
        case 10150:
        case 10151:
        case 10578:
        case 11839:
        case 11921:
        case 11985:
        case 12466:
        case 13140:
        case 13375:
        case 13438:
        case 14034:
        case 15228:
        case 15242:
        case 15536:
        case 15662:
        case 15665:
        case 16101:
        case 16412:
        case 16413:
        case 16415:
        case 16788:
        case 17290:
        case 18082:
        case 18105:
        case 18108:
        case 18199:
        case 18392:
        case 18796:
        case 19391:
        case 19816:
        case 20420:
        case 20678:
        case 20692:
        case 20714:
        case 20793:
        case 20797:
        case 20808:
        case 20811:
        case 20815:
        case 20823:
        case 21072:
        case 21159:
        case 21162:
        case 21402:
        case 21549:
        case 22088:
        case 23024:
        case 23411:
        case 24374:
        case 24611:
        case 25306:
        case 27070:
        case 29456:
        case 29925:
        case 29953:
        case 30218:
        case 30534:
        case 30691:
        case 30943:
        case 30967:
        case 31262:
        case 31620:
        case 32363:
        case 32369:
        case 32414:
        case 32491:
        case 33417:
        case 33793:
        case 33794:
        case 34083:
        case 34348:
        case 34653:
        case 36711:
        case 36805:
        case 36920:
        case 36971:
        case 37111:
        case 37329:
        case 37463:
        case 38641:
        case 38692:
        case 38824:
        case 39267:
        case 40554:
        case 40598:
        case 40877:
        case 41383:
        case 41484:
        case 42802:
        case 42832:
        case 42833:
        case 42834:
        case 42853:
        case 44189:
        case 44202:
        case 44237:
        case 45580:
        case 45595:
        case 45748:
        case 46164:
        case 46988:
        case 47074:
        case 49512:
        case 52282:
        case 54094:
        case 54095:
        case 54096:
        case 57628:
        case 59994:
        case 61567:
        case 61909:
        case 62796:
        case 63789:
        case 63815:
        case 66042:
        case 68310:
        case 68926:
        case 69570:
        case 69583:
        case 69668:
        case 70282:
        case 70409:
        case 70754:
        case 71153:
        case 71500:
        case 71501:
        case 71504:
        case 71748:
        case 71928:
        case 72023:
        case 72024:
        case 72163:
        case 72164:
        {
            sp->custom_NameHash = SPELL_HASH_FIREBALL;
            break;
        }

        case 139:
        case 6074:
        case 6075:
        case 6076:
        case 6077:
        case 6078:
        case 8362:
        case 10927:
        case 10928:
        case 10929:
        case 11640:
        case 22168:
        case 23895:
        case 25058:
        case 25221:
        case 25222:
        case 25315:
        case 27606:
        case 28807:
        case 31325:
        case 34423:
        case 36679:
        case 36969:
        case 37260:
        case 37978:
        case 38210:
        case 41456:
        case 44174:
        case 45859:
        case 46192:
        case 46563:
        case 47079:
        case 48067:
        case 48068:
        case 49263:
        case 56332:
        case 57777:
        case 60004:
        case 61967:
        case 62333:
        case 62441:
        case 66177:
        case 66537:
        case 67675:
        case 68035:
        case 68036:
        case 68037:
        case 71932:
        {
            sp->custom_NameHash = SPELL_HASH_RENEW; break;
        }

        case 184:
        {
            sp->custom_NameHash = SPELL_HASH_FIRE_SHIELD_II; break;
        }

        case 331:
        case 332:
        case 547:
        case 913:
        case 939:
        case 959:
        case 8005:
        case 10395:
        case 10396:
        case 11986:
        case 12491:
        case 12492:
        case 15982:
        case 25357:
        case 25391:
        case 25396:
        case 26097:
        case 38330:
        case 43548:
        case 48700:
        case 49272:
        case 49273:
        case 51586:
        case 52868:
        case 55597:
        case 57785:
        case 58980:
        case 59083:
        case 60012:
        case 61569:
        case 67528:
        case 68318:
        case 69958:
        case 71133:
        case 75382:
        {
            sp->custom_NameHash = SPELL_HASH_HEALING_WAVE; break;
        }

        case 379:
        case 974:
        case 32593:
        case 32594:
        case 32734:
        case 38590:
        case 49283:
        case 49284:
        case 54479:
        case 54480:
        case 55599:
        case 55600:
        case 56451:
        case 57802:
        case 57803:
        case 58981:
        case 58982:
        case 59471:
        case 59472:
        case 60013:
        case 60014:
        case 66063:
        case 66064:
        case 67530:
        case 67537:
        case 68320:
        case 68592:
        case 68593:
        case 68594:
        case 69568:
        case 69569:
        case 69925:
        case 69926:
        {
            sp->custom_NameHash = SPELL_HASH_EARTH_SHIELD; break;
        }

        case 403:
        case 529:
        case 548:
        case 915:
        case 943:
        case 6041:
        case 8246:
        case 9532:
        case 10391:
        case 10392:
        case 12167:
        case 13482:
        case 13527:
        case 14109:
        case 14119:
        case 15207:
        case 15208:
        case 15234:
        case 15801:
        case 16782:
        case 18081:
        case 18089:
        case 19874:
        case 20295:
        case 20802:
        case 20805:
        case 20824:
        case 22414:
        case 23592:
        case 25448:
        case 25449:
        case 26098:
        case 31764:
        case 34345:
        case 35010:
        case 36152:
        case 37273:
        case 37661:
        case 37664:
        case 38465:
        case 39065:
        case 41184:
        case 42024:
        case 43526:
        case 43903:
        case 45075:
        case 45284:
        case 45286:
        case 45287:
        case 45288:
        case 45289:
        case 45290:
        case 45291:
        case 45292:
        case 45293:
        case 45294:
        case 45295:
        case 45296:
        case 48698:
        case 48895:
        case 49237:
        case 49238:
        case 49239:
        case 49240:
        case 49418:
        case 49454:
        case 51587:
        case 51618:
        case 53044:
        case 53314:
        case 54843:
        case 55044:
        case 56326:
        case 56891:
        case 57780:
        case 57781:
        case 59006:
        case 59024:
        case 59081:
        case 59169:
        case 59199:
        case 59683:
        case 59863:
        case 60009:
        case 60032:
        case 61374:
        case 61893:
        case 63809:
        case 64098:
        case 64696:
        case 65987:
        case 68112:
        case 68113:
        case 68114:
        case 69567:
        case 69970:
        case 71136:
        case 71934:
        {
            sp->custom_NameHash = SPELL_HASH_LIGHTNING_BOLT; break;
        }

        case 421:
        case 930:
        case 2860:
        case 10605:
        case 12058:
        case 15117:
        case 15305:
        case 15659:
        case 16006:
        case 16033:
        case 16921:
        case 20831:
        case 21179:
        case 22355:
        case 23106:
        case 23206:
        case 24680:
        case 25021:
        case 25439:
        case 25442:
        case 27567:
        case 28167:
        case 28293:
        case 28900:
        case 31330:
        case 31717:
        case 32337:
        case 33643:
        case 37448:
        case 39066:
        case 39945:
        case 40536:
        case 41183:
        case 42441:
        case 42804:
        case 43435:
        case 44318:
        case 45297:
        case 45298:
        case 45299:
        case 45300:
        case 45301:
        case 45302:
        case 45868:
        case 46380:
        case 48140:
        case 48699:
        case 49268:
        case 49269:
        case 49270:
        case 49271:
        case 50830:
        case 52383:
        case 54334:
        case 54531:
        case 59082:
        case 59220:
        case 59223:
        case 59273:
        case 59517:
        case 59716:
        case 59844:
        case 61528:
        case 61879:
        case 62131:
        case 63479:
        case 64213:
        case 64215:
        case 64390:
        case 64758:
        case 64759:
        case 67529:
        case 68319:
        case 69696:
        case 75362:
        {
            sp->custom_NameHash = SPELL_HASH_CHAIN_LIGHTNING; break;
        }

        case 465:
        case 643:
        case 1032:
        case 8258:
        case 10290:
        case 10291:
        case 10292:
        case 10293:
        case 17232:
        case 27149:
        case 41452:
        case 48941:
        case 48942:
        case 52442:
        case 57740:
        case 58944:
        {
            sp->custom_NameHash = SPELL_HASH_DEVOTION_AURA; break;
        }

        case 498:
        case 13007:
        case 27778:
        case 27779:
        {
            sp->custom_NameHash = SPELL_HASH_DIVINE_PROTECTION; break;
        }

        case 585:
        case 591:
        case 598:
        case 984:
        case 1004:
        case 6060:
        case 10933:
        case 10934:
        case 25363:
        case 25364:
        case 35155:
        case 48122:
        case 48123:
        case 61923:
        case 69967:
        case 71146:
        case 71546:
        case 71547:
        case 71778:
        case 71779:
        case 71841:
        case 71842:
        {
            sp->custom_NameHash = SPELL_HASH_SMITE; break;
        }

        case 605:
        case 11446:
        case 15690:
        case 36797:
        case 36798:
        case 43550:
        case 43871:
        case 43875:
        case 45112:
        case 67229:
        {
            sp->custom_NameHash = SPELL_HASH_MIND_CONTROL; break;
        }

        case 633:
        case 2800:
        case 9257:
        case 10310:
        case 17233:
        case 20233:
        case 20236:
        case 27154:
        case 48788:
        case 53778:
        {
            sp->custom_NameHash = SPELL_HASH_LAY_ON_HANDS; break;
        }

        case 642:
        case 13874:
        case 29382:
        case 33581:
        case 40733:
        case 41367:
        case 54322:
        case 63148:
        case 66010:
        case 67251:
        case 71550:
        {
            sp->custom_NameHash = SPELL_HASH_DIVINE_SHIELD; break;
        }

        case 686:
        case 695:
        case 705:
        case 1088:
        case 1106:
        case 7617:
        case 7619:
        case 7641:
        case 9613:
        case 11659:
        case 11660:
        case 11661:
        case 12471:
        case 12739:
        case 13440:
        case 13480:
        case 14106:
        case 14122:
        case 15232:
        case 15472:
        case 15537:
        case 16408:
        case 16409:
        case 16410:
        case 16783:
        case 16784:
        case 17393:
        case 17434:
        case 17435:
        case 17483:
        case 17509:
        case 18111:
        case 18138:
        case 18164:
        case 18205:
        case 18211:
        case 18214:
        case 18217:
        case 19728:
        case 19729:
        case 20298:
        case 20791:
        case 20807:
        case 20816:
        case 20825:
        case 21077:
        case 21141:
        case 22336:
        case 22677:
        case 24668:
        case 25307:
        case 26006:
        case 27209:
        case 29317:
        case 29487:
        case 29626:
        case 29640:
        case 29927:
        case 30055:
        case 30505:
        case 30686:
        case 31618:
        case 31627:
        case 32666:
        case 32860:
        case 33335:
        case 34344:
        case 36714:
        case 36868:
        case 36972:
        case 36986:
        case 36987:
        case 38378:
        case 38386:
        case 38628:
        case 38825:
        case 38892:
        case 39025:
        case 39026:
        case 39297:
        case 39309:
        case 40185:
        case 41069:
        case 41280:
        case 41957:
        case 43330:
        case 43649:
        case 43667:
        case 45055:
        case 45679:
        case 45680:
        case 47076:
        case 47248:
        case 47808:
        case 47809:
        case 49084:
        case 50455:
        case 51363:
        case 51432:
        case 51608:
        case 52257:
        case 52534:
        case 53086:
        case 53333:
        case 54113:
        case 55984:
        case 56405:
        case 57374:
        case 57464:
        case 57644:
        case 57725:
        case 58827:
        case 59016:
        case 59246:
        case 59254:
        case 59351:
        case 59357:
        case 59389:
        case 59575:
        case 60015:
        case 61558:
        case 61562:
        case 65821:
        case 68151:
        case 68152:
        case 68153:
        case 69028:
        case 69068:
        case 69211:
        case 69212:
        case 69387:
        case 69577:
        case 69972:
        case 70043:
        case 70080:
        case 70182:
        case 70208:
        case 70270:
        case 70386:
        case 70387:
        case 71143:
        case 71254:
        case 71296:
        case 71297:
        case 71936:
        case 72008:
        case 72503:
        case 72504:
        case 72901:
        case 72960:
        case 72961:
        case 75330:
        case 75331:
        case 75384:
        {
            sp->custom_NameHash = SPELL_HASH_SHADOW_BOLT; break;
        }

        case 755:
        case 3698:
        case 3699:
        case 3700:
        case 11693:
        case 11694:
        case 11695:
        case 16569:
        case 27259:
        case 40671:
        case 46467:
        case 47856:
        case 60829:
        {
            sp->custom_NameHash = SPELL_HASH_HEALTH_FUNNEL; break;
        }

        case 770:
        case 6950:
        case 13424:
        case 13752:
        case 16498:
        case 20656:
        case 21670:
        case 25602:
        case 32129:
        case 65863:
        {
            sp->custom_NameHash = SPELL_HASH_FAERIE_FIRE; break;
        }

        case 774:
        case 1058:
        case 1430:
        case 2090:
        case 2091:
        case 3627:
        case 8070:
        case 8910:
        case 9839:
        case 9840:
        case 9841:
        case 12160:
        case 15981:
        case 20664:
        case 20701:
        case 25299:
        case 26981:
        case 26982:
        case 27532:
        case 28716:
        case 28722:
        case 28723:
        case 28724:
        case 31782:
        case 32131:
        case 38657:
        case 42544:
        case 48440:
        case 48441:
        case 53607:
        case 64801:
        case 66065:
        case 67971:
        case 67972:
        case 67973:
        case 69898:
        case 70691:
        case 71142:
        {
            sp->custom_NameHash = SPELL_HASH_REJUVENATION; break;
        }

        case 879:
        case 5614:
        case 5615:
        case 10312:
        case 10313:
        case 10314:
        case 17147:
        case 17149:
        case 27138:
        case 33632:
        case 48800:
        case 48801:
        case 52445:
        case 58822:
        {
            sp->custom_NameHash = SPELL_HASH_EXORCISM; break;
        }

        case 980:
        case 1014:
        case 6217:
        case 11711:
        case 11712:
        case 11713:
        case 14868:
        case 14875:
        case 17771:
        case 18266:
        case 18671:
        case 27218:
        case 29930:
        case 32418:
        case 37334:
        case 39672:
        case 46190:
        case 47863:
        case 47864:
        case 65814:
        case 68136:
        case 68137:
        case 68138:
        case 69404:
        case 70391:
        case 71112:
        {
            sp->custom_NameHash = SPELL_HASH_CURSE_OF_AGONY; break;
        }

        case 1064:
        case 10622:
        case 10623:
        case 14900:
        case 15799:
        case 16367:
        case 25422:
        case 25423:
        case 33642:
        case 41114:
        case 42027:
        case 42477:
        case 43527:
        case 48894:
        case 54481:
        case 55458:
        case 55459:
        case 59473:
        case 69923:
        case 70425:
        case 71120:
        case 75370:
        {
            sp->custom_NameHash = SPELL_HASH_CHAIN_HEAL; break;
        }

        case 34453:
        case 34454:
        case 68361:
        {
            sp->custom_NameHash = SPELL_HASH_ANIMAL_HANDLER; break;
        }
        case 44425:
        case 44780:
        case 44781:
        case 50273:
        case 50804:
        case 56397:
        case 58456:
        case 59248:
        case 59381:
        case 63934:
        case 64599:
        case 64607:
        case 65799:
        case 67994:
        case 67995:
        case 67996:
        {
            sp->custom_NameHash = SPELL_HASH_ARCANE_BARRAGE; break;
        }
        case 24544:
        case 31571:
        case 31572:
        case 33421:
        case 33713:
        case 57529:
        case 57531:
        {
            sp->custom_NameHash = SPELL_HASH_ARCANE_POTENCY; break;
        }
        case 3044:
        case 14281:
        case 14282:
        case 14283:
        case 14284:
        case 14285:
        case 14286:
        case 14287:
        case 27019:
        case 34829:
        case 35401:
        case 36293:
        case 36609:
        case 36623:
        case 38807:
        case 49044:
        case 49045:
        case 51742:
        case 55624:
        case 58973:
        case 69989:
        case 71116:
        {
            sp->custom_NameHash = SPELL_HASH_ARCANE_SHOT; break;
        }
        case 31850:
        case 31851:
        case 31852:
        case 66233:
        case 66235:
        {
            sp->custom_NameHash = SPELL_HASH_ARDENT_DEFENDER; break;
        }
        case 20655:
        case 22812:
        case 63408:
        case 63409:
        case 65860:
        {
            sp->custom_NameHash = SPELL_HASH_BARKSKIN; break;
        }
        case 18499:
        {
            sp->custom_NameHash = SPELL_HASH_BERSERKER_RAGE; break;
        }
        case 19590:
        case 19592:
        {
            sp->custom_NameHash = SPELL_HASH_BESTIAL_DISCIPLINE; break;
        }
        case 40475:
        {
            sp->custom_NameHash = SPELL_HASH_BLACK_TEMPLE_MELEE_TRINKET; break;
        }
        case 13877:
        case 22482:
        case 33735:
        case 44181:
        case 51211:
        case 65956:
        {
            sp->custom_NameHash = SPELL_HASH_BLADE_FLURRY; break;
        }
        case 9632:
        case 35131:
        case 46924:
        case 63784:
        case 63785:
        case 65946:
        case 65947:
        case 67541:
        case 69652:
        case 69653:
        {
            sp->custom_NameHash = SPELL_HASH_BLADESTORM; break;
        }
        case 41450:
        {
            sp->custom_NameHash = SPELL_HASH_BLESSING_OF_PROTECTION; break;
        }

        case 16952:
        case 16954:
        case 29836:
        case 29859:
        case 30069:
        case 30070:
        {
            sp->custom_NameHash = SPELL_HASH_BLOOD_FRENZY; break;
        }
        case 2825:
        case 6742:
        case 16170:
        case 21049:
        case 23951:
        case 24185:
        case 27689:
        case 28902:
        case 33555:
        case 37067:
        case 37309:
        case 37310:
        case 37472:
        case 37599:
        case 41185:
        case 43578:
        case 45584:
        case 50730:
        case 54516:
        case 65980:
        {
            sp->custom_NameHash = SPELL_HASH_BLOODLUST; break;
        }

        case 34462:
        case 34464:
        case 34465:
        {
            sp->custom_NameHash = SPELL_HASH_CATLIKE_REFLEXES; break;
        }
        case 50796:
        case 51287:
        case 59170:
        case 59171:
        case 59172:
        case 69576:
        case 71108:
        {
            sp->custom_NameHash = SPELL_HASH_CHAOS_BOLT; break;
        }

        case 4987:
        case 28787:
        case 28788:
        case 29380:
        case 32400:
        case 39078:
        case 57767:
        case 66116:
        case 68621:
        case 68622:
        case 68623:
        {
            sp->custom_NameHash = SPELL_HASH_CLEANSE; break;
        }
        case 12536:
        case 16246:
        case 16870:
        case 67210:
        {
            sp->custom_NameHash = SPELL_HASH_CLEARCASTING; break;
        }
        case 11129:
        case 28682:
        case 29977:
        case 74630:
        case 75882:
        case 75883:
        case 75884:
        {
            sp->custom_NameHash = SPELL_HASH_COMBUSTION; break;
        }
        case 19746:
        {
            sp->custom_NameHash = SPELL_HASH_CONCENTRATION_AURA; break;
        }

        case 17962:
        {
            sp->custom_NameHash = SPELL_HASH_CONFLAGRATE; break;
        }
        case 32223:
        {
            sp->custom_NameHash = SPELL_HASH_CRUSADER_AURA; break;
        }
        case 14517:
        case 14518:
        case 17281:
        case 35395:
        case 35509:
        case 36647:
        case 50773:
        case 65166:
        case 66003:
        case 71549:
        {
            sp->custom_NameHash = SPELL_HASH_CRUSADER_STRIKE; break;
        }

        case 1604:
        case 5101:
        case 13496:
        case 15571:
        case 29703:
        case 35955:
        case 50259:
        case 50411:
        case 51372:
        {
            sp->custom_NameHash = SPELL_HASH_DAZED; break;
        }

        case 17471:
        case 17698:
        case 48743:
        case 51956:
        {
            sp->custom_NameHash = SPELL_HASH_DEATH_PACT; break;
        }
        case 12292:
        {
            sp->custom_NameHash = SPELL_HASH_DEATH_WISH; break;
        }
        case 18788:
        {
            sp->custom_NameHash = SPELL_HASH_DEMONIC_SACRIFICE; break;
        }

        case 47218:
        case 47585:
        case 49766:
        case 49768:
        case 60069:
        case 63230:
        case 65544:
        {
            sp->custom_NameHash = SPELL_HASH_DISPERSION; break;
        }
        case 53385:
        case 54171:
        case 54172:
        case 58127:
        case 66006:
        {
            sp->custom_NameHash = SPELL_HASH_DIVINE_STORM; break;
        }

        case 8042:
        case 8044:
        case 8045:
        case 8046:
        case 10412:
        case 10413:
        case 10414:
        case 13281:
        case 13728:
        case 15501:
        case 22885:
        case 23114:
        case 24685:
        case 25025:
        case 25454:
        case 26194:
        case 43305:
        case 47071:
        case 49230:
        case 49231:
        case 54511:
        case 56506:
        case 57783:
        case 60011:
        case 61668:
        case 65973:
        case 68100:
        case 68101:
        case 68102:
        {
            sp->custom_NameHash = SPELL_HASH_EARTH_SHOCK; break;
        }
        case 19583:
        case 19584:
        case 19585:
        case 19586:
        case 19587:
        {
            sp->custom_NameHash = SPELL_HASH_ENDURANCE_TRAINING; break;
        }

        case 59752:
        {
            sp->custom_NameHash = SPELL_HASH_EVERY_MAN_FOR_HIMSELF; break;
        }

        case 16857:
        case 60089:
        {
            sp->custom_NameHash = SPELL_HASH_FAERIE_FIRE__FERAL_; break;
        }

        case 6346:
        {
            sp->custom_NameHash = SPELL_HASH_FEAR_WARD; break;
        }
        case 17002:
        case 24866:
        {
            sp->custom_NameHash = SPELL_HASH_FERAL_SWIFTNESS; break;
        }

        case 4154:
        case 16934:
        case 16935:
        case 16936:
        case 16937:
        case 16938:
        case 19598:
        case 19599:
        case 19600:
        case 19601:
        case 19602:
        case 33667:
        {
            sp->custom_NameHash = SPELL_HASH_FEROCITY; break;
        }
        case 19891:
        case 19899:
        case 19900:
        case 27153:
        case 48947:
        {
            sp->custom_NameHash = SPELL_HASH_FIRE_RESISTANCE_AURA; break;
        }
        case 7712:
        case 7714:
        case 7715:
        case 7716:
        case 7717:
        case 7718:
        case 7719:
        {
            sp->custom_NameHash = SPELL_HASH_FIRE_STRIKE; break;
        }
        case 8050:
        case 8052:
        case 8053:
        case 10447:
        case 10448:
        case 13729:
        case 15039:
        case 15096:
        case 15616:
        case 16804:
        case 22423:
        case 23038:
        case 25457:
        case 29228:
        case 32967:
        case 34354:
        case 39529:
        case 39590:
        case 41115:
        case 43303:
        case 49232:
        case 49233:
        case 51588:
        case 55613:
        case 58940:
        case 58971:
        case 59684:
        {
            sp->custom_NameHash = SPELL_HASH_FLAME_SHOCK; break;
        }
        case 19750:
        case 19939:
        case 19940:
        case 19941:
        case 19942:
        case 19943:
        case 25514:
        case 27137:
        case 33641:
        case 37249:
        case 37254:
        case 37257:
        case 48784:
        case 48785:
        case 57766:
        case 59997:
        case 66113:
        case 66922:
        case 68008:
        case 68009:
        case 68010:
        case 71930:
        {
            sp->custom_NameHash = SPELL_HASH_FLASH_OF_LIGHT; break;
        }

        case 19888:
        case 19897:
        case 19898:
        case 27152:
        case 48945:
        {
            sp->custom_NameHash = SPELL_HASH_FROST_RESISTANCE_AURA; break;
        }
        case 8056:
        case 8058:
        case 10472:
        case 10473:
        case 12548:
        case 15089:
        case 15499:
        case 19133:
        case 21030:
        case 21401:
        case 22582:
        case 23115:
        case 25464:
        case 29666:
        case 34353:
        case 37332:
        case 37865:
        case 38234:
        case 39062:
        case 41116:
        case 43524:
        case 46180:
        case 49235:
        case 49236:
        {
            sp->custom_NameHash = SPELL_HASH_FROST_SHOCK; break;
        }

        case 44614:
        case 47610:
        case 51779:
        case 69869:
        case 69984:
        case 70616:
        case 71130:
        {
            sp->custom_NameHash = SPELL_HASH_FROSTFIRE_BOLT; break;
        }

        case 4102:
        case 32019:
        case 35290:
        case 35291:
        case 35292:
        case 35293:
        case 35294:
        case 35295:
        case 35299:
        case 35300:
        case 35302:
        case 35303:
        case 35304:
        case 35305:
        case 35306:
        case 35307:
        case 35308:
        case 48130:
        case 51751:
        case 59264:
        {
            sp->custom_NameHash = SPELL_HASH_GORE; break;
        }

        case 24239:
        case 24274:
        case 24275:
        case 27180:
        case 32772:
        case 37251:
        case 37255:
        case 37259:
        case 48805:
        case 48806:
        case 51384:
        {
            sp->custom_NameHash = SPELL_HASH_HAMMER_OF_WRATH; break;
        }

        case 48181:
        case 48184:
        case 48210:
        case 50091:
        case 59161:
        case 59163:
        case 59164:
        {
            sp->custom_NameHash = SPELL_HASH_HAUNT; break;
        }
        case 17003:
        case 17004:
        case 17005:
        case 17006:
        case 24894:
        {
            sp->custom_NameHash = SPELL_HASH_HEART_OF_THE_WILD; break;
        }
        case 5857:
        case 11681:
        case 11682:
        case 27214:
        case 30860:
        case 47822:
        case 65817:
        case 68142:
        case 68143:
        case 68144:
        case 69585:
        case 70284:
        {
            sp->custom_NameHash = SPELL_HASH_HELLFIRE_EFFECT; break;
        }
        case 16511:
        case 17347:
        case 17348:
        case 26864:
        case 30478:
        case 37331:
        case 45897:
        case 48660:
        case 65954:
        {
            sp->custom_NameHash = SPELL_HASH_HEMORRHAGE; break;
        }

        case 23682:
        case 23689:
        case 32182:
        case 32927:
        case 32955:
        case 37471:
        case 39200:
        case 65983:
        {
            sp->custom_NameHash = SPELL_HASH_HEROISM; break;
        }

        case 20473:
        case 20929:
        case 20930:
        case 25902:
        case 25903:
        case 25911:
        case 25912:
        case 25913:
        case 25914:
        case 27174:
        case 27175:
        case 27176:
        case 32771:
        case 33072:
        case 33073:
        case 33074:
        case 35160:
        case 36340:
        case 38921:
        case 48820:
        case 48821:
        case 48822:
        case 48823:
        case 48824:
        case 48825:
        case 66114:
        case 68014:
        case 68015:
        case 68016:
        {
            sp->custom_NameHash = SPELL_HASH_HOLY_SHOCK; break;
        }

        case 27619:
        case 36911:
        case 41590:
        case 45438:
        case 45776:
        case 46604:
        case 46882:
        case 56124:
        case 56644:
        case 62766:
        case 65802:
        case 69924:
        {
            sp->custom_NameHash = SPELL_HASH_ICE_BLOCK; break;
        }

        case 3261:
        case 11119:
        case 11120:
        case 12654:
        case 12846:
        case 12847:
        case 12848:
        case 52210:
        case 58438:
        {
            sp->custom_NameHash = SPELL_HASH_IGNITE; break;
        }

        case 62905:
        case 62908:
        {
            sp->custom_NameHash = SPELL_HASH_IMPROVED_DEATH_STRIKE; break;
        }

        case 59088:
        case 59089:
        {
            sp->custom_NameHash = SPELL_HASH_IMPROVED_SPELL_REFLECTION; break;
        }

        case 19397:
        case 23308:
        case 23309:
        case 29722:
        case 32231:
        case 32707:
        case 36832:
        case 38401:
        case 38918:
        case 39083:
        case 40239:
        case 41960:
        case 43971:
        case 44519:
        case 46043:
        case 47837:
        case 47838:
        case 53493:
        case 69973:
        case 71135:
        {
            sp->custom_NameHash = SPELL_HASH_INCINERATE; break;
        }

        case 31898:
        case 32220:
        case 41461:
        {
            sp->custom_NameHash = SPELL_HASH_JUDGEMENT_OF_BLOOD; break;
        }
        case 20425:
        case 20467:
        case 29386:
        case 32778:
        case 33554:
        case 41368:
        case 41470:
        case 66005:
        case 68017:
        case 68018:
        case 68019:
        case 71551:
        {
            sp->custom_NameHash = SPELL_HASH_JUDGEMENT_OF_COMMAND; break;
        }
        case 48492:
        case 48494:
        case 48495:
        case 51178:
        case 51185:
        {
            sp->custom_NameHash = SPELL_HASH_KING_OF_THE_JUNGLE; break;
        }
        case 21158:
        case 51505:
        case 53788:
        case 55659:
        case 55704:
        case 56491:
        case 58972:
        case 59182:
        case 59519:
        case 60043:
        case 61924:
        case 64870:
        case 64991:
        case 66813:
        case 67330:
        case 71824:
        {
            sp->custom_NameHash = SPELL_HASH_LAVA_BURST; break;
        }
        case 8004:
        case 8008:
        case 8010:
        case 10466:
        case 10467:
        case 10468:
        case 25420:
        case 27624:
        case 28849:
        case 28850:
        case 44256:
        case 46181:
        case 49275:
        case 49276:
        case 49309:
        case 66055:
        case 68115:
        case 68116:
        case 68117:
        case 75366:
        {
            sp->custom_NameHash = SPELL_HASH_LESSER_HEALING_WAVE; break;
        }
        case 1454:
        case 1455:
        case 1456:
        case 4090:
        case 11687:
        case 11688:
        case 11689:
        case 27222:
        case 28830:
        case 31818:
        case 32553:
        case 57946:
        case 63321:
        {
            sp->custom_NameHash = SPELL_HASH_LIFE_TAP; break;
        }
        case 16614:
        case 23686:
        case 23687:
        case 27983:
        case 37841:
        case 52944:
        case 53062:
        {
            sp->custom_NameHash = SPELL_HASH_LIGHTNING_STRIKE; break;
        }
        case 7001:
        case 27873:
        case 27874:
        case 28276:
        case 48084:
        case 48085:
        case 60123:
        {
            sp->custom_NameHash = SPELL_HASH_LIGHTWELL_RENEW; break;
        }
        case 5530:
        case 12284:
        case 12701:
        case 12702:
        case 12703:
        case 12704:
        case 13709:
        case 13800:
        case 13801:
        case 13802:
        case 13803:
        case 20864:
        case 59224:
        {
            sp->custom_NameHash = SPELL_HASH_MACE_SPECIALIZATION; break;
        }
        case 34774:
        {
            sp->custom_NameHash = SPELL_HASH_MAGTHERIDON_MELEE_TRINKET; break;
        }

        case 31221:
        case 31222:
        case 31223:
        case 31665:
        case 31666:
        {
            sp->custom_NameHash = SPELL_HASH_MASTER_OF_SUBTLETY; break;
        }
        case 45150:
        {
            sp->custom_NameHash = SPELL_HASH_METEOR_SLASH; break;
        }
        case 30482:
        case 34913:
        case 35915:
        case 35916:
        case 43043:
        case 43044:
        case 43045:
        case 43046:
        {
            sp->custom_NameHash = SPELL_HASH_MOLTEN_ARMOR; break;
        }

        case 1329:
        case 5374:
        case 27576:
        case 32319:
        case 32320:
        case 32321:
        case 34411:
        case 34412:
        case 34413:
        case 34414:
        case 34415:
        case 34416:
        case 34417:
        case 34418:
        case 34419:
        case 41103:
        case 48661:
        case 48662:
        case 48663:
        case 48664:
        case 48665:
        case 48666:
        case 60850:
        {
            sp->custom_NameHash = SPELL_HASH_MUTILATE; break;
        }
        case 58426:
        case 58427:
        {
            sp->custom_NameHash = SPELL_HASH_OVERKILL; break;
        }

        case 12043:
        case 29976:
        {
            sp->custom_NameHash = SPELL_HASH_PRESENCE_OF_MIND; break;
        }

        case 52942:
        case 52961:
        case 59836:
        case 59837:
        {
            sp->custom_NameHash = SPELL_HASH_PULSING_SHOCKWAVE; break;
        }
        case 42292:
        case 65547:
        {
            sp->custom_NameHash = SPELL_HASH_PVP_TRINKET; break;
        }

        case 4629:
        case 5740:
        case 6219:
        case 11677:
        case 11678:
        case 11990:
        case 16005:
        case 19474:
        case 19717:
        case 20754:
        case 24669:
        case 27212:
        case 28794:
        case 31340:
        case 31598:
        case 33508:
        case 33617:
        case 33627:
        case 33972:
        case 34169:
        case 34185:
        case 34360:
        case 34435:
        case 36808:
        case 37279:
        case 37465:
        case 38635:
        case 38741:
        case 39024:
        case 39273:
        case 39363:
        case 39376:
        case 42023:
        case 42218:
        case 42223:
        case 42224:
        case 42225:
        case 42226:
        case 42227:
        case 43440:
        case 47817:
        case 47818:
        case 47819:
        case 47820:
        case 49518:
        case 54099:
        case 54210:
        case 57757:
        case 58936:
        case 59971:
        case 69670:
        {
            sp->custom_NameHash = SPELL_HASH_RAIN_OF_FIRE; break;
        }

        case 34948:
        case 34949:
        case 35098:
        case 35099:
        {
            sp->custom_NameHash = SPELL_HASH_RAPID_KILLING; break;
        }
        case 53228:
        case 53232:
        case 56654:
        case 58882:
        case 58883:
        case 64180:
        case 64181:
        {
            sp->custom_NameHash = SPELL_HASH_RAPID_RECUPERATION; break;
        }

        case 8936:
        case 8938:
        case 8939:
        case 8940:
        case 8941:
        case 9750:
        case 9856:
        case 9857:
        case 9858:
        case 16561:
        case 20665:
        case 22373:
        case 22695:
        case 26980:
        case 27637:
        case 28744:
        case 34361:
        case 39000:
        case 39125:
        case 48442:
        case 48443:
        case 66067:
        case 67968:
        case 67969:
        case 67970:
        case 69882:
        case 71141:
        {
            sp->custom_NameHash = SPELL_HASH_REGROWTH; break;
        }

        case 5405:
        case 10052:
        case 10057:
        case 10058:
        case 18385:
        case 27103:
        case 33394:
        case 42987:
        case 42988:
        case 71565:
        case 71574:
        {
            sp->custom_NameHash = SPELL_HASH_REPLENISH_MANA; break;
        }
        case 46699:
        {
            sp->custom_NameHash = SPELL_HASH_REQUIRES_NO_AMMO; break;
        }
        case 7294:
        case 8990:
        case 10298:
        case 10299:
        case 10300:
        case 10301:
        case 13008:
        case 27150:
        case 54043:
        {
            sp->custom_NameHash = SPELL_HASH_RETRIBUTION_AURA; break;
        }

        case 22419:
        case 61295:
        case 61299:
        case 61300:
        case 61301:
        case 66053:
        case 68118:
        case 68119:
        case 68120:
        case 75367:
        {
            sp->custom_NameHash = SPELL_HASH_RIPTIDE; break;
        }
        case 34586:
        case 34587:
        {
            sp->custom_NameHash = SPELL_HASH_ROMULO_S_POISON; break;
        }

        case 53601:
        case 58597:
        {
            sp->custom_NameHash = SPELL_HASH_SACRED_SHIELD; break;
        }

        case 1811:
        case 2948:
        case 8444:
        case 8445:
        case 8446:
        case 8447:
        case 8448:
        case 8449:
        case 10205:
        case 10206:
        case 10207:
        case 10208:
        case 10209:
        case 10210:
        case 13878:
        case 15241:
        case 17195:
        case 27073:
        case 27074:
        case 27375:
        case 27376:
        case 35377:
        case 36807:
        case 38391:
        case 38636:
        case 42858:
        case 42859:
        case 47723:
        case 50183:
        case 56938:
        case 62546:
        case 62548:
        case 62549:
        case 62551:
        case 62553:
        case 63473:
        case 63474:
        case 63475:
        case 63476:
        case 75412:
        case 75419:
        {
            sp->custom_NameHash = SPELL_HASH_SCORCH; break;
        }

        case 5676:
        case 17919:
        case 17920:
        case 17921:
        case 17922:
        case 17923:
        case 27210:
        case 29492:
        case 30358:
        case 30459:
        case 47814:
        case 47815:
        case 65819:
        case 68148:
        case 68149:
        case 68150:
        {
            sp->custom_NameHash = SPELL_HASH_SEARING_PAIN; break;
        }

        case 27243:
        case 27285:
        case 32863:
        case 32865:
        case 36123:
        case 37826:
        case 38252:
        case 39367:
        case 43991:
        case 44141:
        case 47831:
        case 47832:
        case 47833:
        case 47834:
        case 47835:
        case 47836:
        case 70388:
        {
            sp->custom_NameHash = SPELL_HASH_SEED_OF_CORRUPTION; break;
        }
        case 34466:
        case 34467:
        case 34468:
        case 34469:
        case 34470:
        {
            sp->custom_NameHash = SPELL_HASH_SERPENT_S_SWIFTNESS; break;
        }
        case 14171:
        case 14172:
        case 14173:
        {
            sp->custom_NameHash = SPELL_HASH_SERRATED_BLADES; break;
        }
        case 19876:
        case 19895:
        case 19896:
        case 27151:
        case 48943:
        {
            sp->custom_NameHash = SPELL_HASH_SHADOW_RESISTANCE_AURA; break;
        }
        case 17877:
        case 18867:
        case 18868:
        case 18869:
        case 18870:
        case 18871:
        case 27263:
        case 29341:
        case 30546:
        case 47826:
        case 47827:
        {
            sp->custom_NameHash = SPELL_HASH_SHADOWBURN; break;
        }

        case 53600:
        case 61411:
        {
            sp->custom_NameHash = SPELL_HASH_SHIELD_OF_RIGHTEOUSNESS; break;
        }

        case 25425:
        case 33686:
        case 46968:
        case 55636:
        case 55918:
        case 57728:
        case 57741:
        case 58947:
        case 58977:
        case 63783:
        case 63982:
        case 72149:
        case 73499:
        case 73794:
        case 73795:
        case 73796:
        case 75343:
        case 75417:
        case 75418:
        {
            sp->custom_NameHash = SPELL_HASH_SHOCKWAVE; break;
        }

        case 35195:
        case 41597:
        case 63106:
        case 63108:
        {
            sp->custom_NameHash = SPELL_HASH_SIPHON_LIFE; break;
        }
        case 6353:
        case 17924:
        case 27211:
        case 30545:
        case 47824:
        case 47825:
        {
            sp->custom_NameHash = SPELL_HASH_SOUL_FIRE; break;
        }
        case 2912:
        case 8949:
        case 8950:
        case 8951:
        case 9875:
        case 9876:
        case 21668:
        case 25298:
        case 26986:
        case 35243:
        case 38935:
        case 40344:
        case 48464:
        case 48465:
        case 65854:
        case 67947:
        case 67948:
        case 67949:
        case 75332:
        {
            sp->custom_NameHash = SPELL_HASH_STARFIRE; break;
        }

        case 1784:
        case 1785:
        case 1786:
        case 1787:
        case 8822:
        case 30831:
        case 30991:
        case 31526:
        case 31621:
        case 32199:
        case 32615:
        case 34189:
        case 42347:
        case 42866:
        case 42943:
        case 52188:
        case 58506:
        {
            sp->custom_NameHash = SPELL_HASH_STEALTH; break;
        }

        case 7386:
        case 7405:
        case 8380:
        case 11596:
        case 11597:
        case 11971:
        case 13444:
        case 15502:
        case 15572:
        case 16145:
        case 21081:
        case 24317:
        case 25051:
        case 25225:
        case 27991:
        case 30901:
        case 47467:
        case 48893:
        case 50370:
        case 53618:
        case 54188:
        case 57807:
        case 58461:
        case 58567:
        case 59350:
        case 59608:
        case 64978:
        case 65936:
        case 71554:
        {
            sp->custom_NameHash = SPELL_HASH_SUNDER_ARMOR; break;
        }

        case 34471:
        case 34692:
        case 38373:
        case 50098:
        case 70029:
        {
            sp->custom_NameHash = SPELL_HASH_THE_BEAST_WITHIN; break;
        }
        case 16929:
        case 16930:
        case 16931:
        case 19609:
        case 19610:
        case 19612:
        case 50502:
        {
            sp->custom_NameHash = SPELL_HASH_THICK_HIDE; break;
        }
        case 6343:
        case 8198:
        case 8204:
        case 8205:
        case 11580:
        case 11581:
        case 13532:
        case 25264:
        case 47501:
        case 47502:
        case 57832:
        case 60019:
        {
            sp->custom_NameHash = SPELL_HASH_THUNDER_CLAP; break;
        }

        case 19616:
        case 19617:
        case 19618:
        case 19619:
        case 19620:
        {
            sp->custom_NameHash = SPELL_HASH_UNLEASHED_FURY; break;
        }
        case 30108:
        case 30404:
        case 30405:
        case 31117:
        case 34438:
        case 34439:
        case 35183:
        case 43522:
        case 43523:
        case 47841:
        case 47843:
        case 65812:
        case 65813:
        case 68154:
        case 68155:
        case 68156:
        case 68157:
        case 68158:
        case 68159:
        {
            sp->custom_NameHash = SPELL_HASH_UNSTABLE_AFFLICTION; break;
        }
        case 1856:
        case 1857:
        case 11327:
        case 11329:
        case 24223:
        case 24228:
        case 24229:
        case 24230:
        case 24231:
        case 24232:
        case 24233:
        case 24699:
        case 26888:
        case 26889:
        case 27617:
        case 29448:
        case 31619:
        case 35205:
        case 39667:
        case 41476:
        case 41479:
        case 44290:
        case 55964:
        case 71400:
        {
            sp->custom_NameHash = SPELL_HASH_VANISH;
            break;
        }

        case 7744:
        {
            sp->custom_NameHash = SPELL_HASH_WILL_OF_THE_FORSAKEN;
            break;
        }
        case 8232:
        case 8235:
        case 10486:
        case 16362:
        case 25505:
        case 32911:
        case 35886:
        case 58801:
        case 58803:
        case 58804:
        {
            sp->custom_NameHash = SPELL_HASH_WINDFURY_WEAPON;
            break;
        }
        case 5176:
        case 5177:
        case 5178:
        case 5179:
        case 5180:
        case 6780:
        case 8905:
        case 9739:
        case 9912:
        case 17144:
        case 18104:
        case 20698:
        case 21667:
        case 21807:
        case 26984:
        case 26985:
        case 31784:
        case 43619:
        case 48459:
        case 48461:
        case 52501:
        case 57648:
        case 59986:
        case 62793:
        case 63259:
        case 63569:
        case 65862:
        case 67951:
        case 67952:
        case 67953:
        case 69968:
        case 71148:
        case 75327:
        {
            sp->custom_NameHash = SPELL_HASH_WRATH;
            break;
        }
        default:
            break;
    }
}

void ApplyNormalFixes()
{
    //Updating spell.dbc

    LogNotice("World : Processing %u spells...", sSpellCustomizations.GetSpellInfoStore()->size());

    SpellInfo* sp = nullptr;

    for (auto it = sSpellCustomizations.GetSpellInfoStore()->begin(); it != sSpellCustomizations.GetSpellInfoStore()->end(); ++it)
    {
        // Read every SpellEntry row
        sp = sSpellCustomizations.GetSpellInfo(it->first);
        if (sp == nullptr)
            continue;

        sp->custom_NameHash = 0;

        //apply custom name hash to spell id
        ApplyObsoleteNameHash(sp);

        float radius = std::max(::GetRadius(sSpellRadiusStore.LookupEntry(sp->EffectRadiusIndex[0])), ::GetRadius(sSpellRadiusStore.LookupEntry(sp->EffectRadiusIndex[1])));
        radius = std::max(::GetRadius(sSpellRadiusStore.LookupEntry(sp->EffectRadiusIndex[2])), radius);
        radius = std::max(GetMaxRange(sSpellRangeStore.LookupEntry(sp->rangeIndex)), radius);
        sp->custom_base_range_or_radius_sqr = radius * radius;

        sp->ai_target_type = sp->aiTargetType();
        // NEW SCHOOLS AS OF 2.4.0:
        // (bitwise)
        //SCHOOL_NORMAL = 1,
        //SCHOOL_HOLY   = 2,
        //SCHOOL_FIRE   = 4,
        //SCHOOL_NATURE = 8,
        //SCHOOL_FROST  = 16,
        //SCHOOL_SHADOW = 32,
        //SCHOOL_ARCANE = 64

        // Save School as custom_SchoolMask, and set School as an index
        sp->custom_SchoolMask = sp->School;
        for (uint8 i = 0; i < SCHOOL_COUNT; ++i)
        {
            if (sp->School & (1 << i))
            {
                sp->School = i;
                break;
            }
        }

        ARCEMU_ASSERT(sp->School < SCHOOL_COUNT);

        // correct caster/target aura states
        if (sp->CasterAuraState > 1)
            sp->CasterAuraState = 1 << (sp->CasterAuraState - 1);

        if (sp->TargetAuraState > 1)
            sp->TargetAuraState = 1 << (sp->TargetAuraState - 1);


        //there are some spells that change the "damage" value of 1 effect to another : devastate = bonus first then damage
        //this is a total bullshit so remove it when spell system supports effect overwriting
        switch (sp->getId())
        {
            case 20243:     // Devastate Rank 1
            case 30016:     // Devastate Rank 2
            case 30022:     // Devastate Rank 3
            case 47497:     // Devastate Rank 4
            case 47498:     // Devastate Rank 5
            case 57795:     // Devastate
            case 60018:     // Devastate
            case 62317:     // Devastate
            case 69902:     // Devastate
            {
                uint32 temp;
                float ftemp;
                temp = sp->Effect[1];
                sp->Effect[1] = sp->Effect[2];
                sp->Effect[2] = temp;
                temp = sp->EffectDieSides[1];
                sp->EffectDieSides[1] = sp->EffectDieSides[2];
                sp->EffectDieSides[2] = temp;
                //temp = sp->EffectBaseDice[1];    sp->EffectBaseDice[1] = sp->EffectBaseDice[2] ;        sp->EffectBaseDice[2] = temp;
                //ftemp = sp->EffectDicePerLevel[1];            sp->EffectDicePerLevel[1] = sp->EffectDicePerLevel[2] ;                sp->EffectDicePerLevel[2] = ftemp;
                ftemp = sp->EffectRealPointsPerLevel[1];
                sp->EffectRealPointsPerLevel[1] = sp->EffectRealPointsPerLevel[2];
                sp->EffectRealPointsPerLevel[2] = ftemp;
                temp = sp->EffectBasePoints[1];
                sp->EffectBasePoints[1] = sp->EffectBasePoints[2];
                sp->EffectBasePoints[2] = temp;
                temp = sp->EffectMechanic[1];
                sp->EffectMechanic[1] = sp->EffectMechanic[2];
                sp->EffectMechanic[2] = temp;
                temp = sp->EffectImplicitTargetA[1];
                sp->EffectImplicitTargetA[1] = sp->EffectImplicitTargetA[2];
                sp->EffectImplicitTargetA[2] = temp;
                temp = sp->EffectImplicitTargetB[1];
                sp->EffectImplicitTargetB[1] = sp->EffectImplicitTargetB[2];
                sp->EffectImplicitTargetB[2] = temp;
                temp = sp->EffectRadiusIndex[1];
                sp->EffectRadiusIndex[1] = sp->EffectRadiusIndex[2];
                sp->EffectRadiusIndex[2] = temp;
                temp = sp->EffectApplyAuraName[1];
                sp->EffectApplyAuraName[1] = sp->EffectApplyAuraName[2];
                sp->EffectApplyAuraName[2] = temp;
                temp = sp->EffectAmplitude[1];
                sp->EffectAmplitude[1] = sp->EffectAmplitude[2];
                sp->EffectAmplitude[2] = temp;
                ftemp = sp->EffectMultipleValue[1];
                sp->EffectMultipleValue[1] = sp->EffectMultipleValue[2];
                sp->EffectMultipleValue[2] = ftemp;
                temp = sp->EffectChainTarget[1];
                sp->EffectChainTarget[1] = sp->EffectChainTarget[2];
                sp->EffectChainTarget[2] = temp;
                temp = sp->EffectMiscValue[1];
                sp->EffectMiscValue[1] = sp->EffectMiscValue[2];
                sp->EffectMiscValue[2] = temp;
                temp = sp->EffectTriggerSpell[1];
                sp->EffectTriggerSpell[1] = sp->EffectTriggerSpell[2];
                sp->EffectTriggerSpell[2] = temp;
                ftemp = sp->EffectPointsPerComboPoint[1];
                sp->EffectPointsPerComboPoint[1] = sp->EffectPointsPerComboPoint[2];
                sp->EffectPointsPerComboPoint[2] = ftemp;
            } break;
            default:
                break;
        }

        for (uint32 b = 0; b < 3; ++b)
        {
            if (sp->EffectTriggerSpell[b] != 0 && sSpellCustomizations.GetSpellInfo(sp->EffectTriggerSpell[b]) == NULL)
            {
                // proc spell referencing non-existent spell. create a dummy spell for use w/ it.
                CreateDummySpell(sp->EffectTriggerSpell[b]);
            }

            if (sp->Attributes & ATTRIBUTES_ONLY_OUTDOORS && sp->EffectApplyAuraName[b] == SPELL_AURA_MOUNTED)
            {
                sp->Attributes &= ~ATTRIBUTES_ONLY_OUTDOORS;
            }

            if (sp->EffectApplyAuraName[b] == SPELL_AURA_PREVENT_RESURRECTION)
			{
				sp->Attributes |= ATTRIBUTES_NEGATIVE;
				sp->AttributesExC |= ATTRIBUTESEXC_CAN_PERSIST_AND_CASTED_WHILE_DEAD;
			}
        }

        sp->Dspell_coef_override = -1;
        sp->OTspell_coef_override = -1;
        sp->casttime_coef = 0;
        sp->fixed_dddhcoef = -1;
        sp->fixed_hotdotcoef = -1;


        // DankoDJ: Refactoring session 16/02/2016 new functions
        Modify_EffectBasePoints(sp);
        Set_missing_spellLevel(sp);
        Modify_AuraInterruptFlags(sp);
        Modify_RecoveryTime(sp);

        // find diminishing status
        //\todo 16/03/08 Zyres: sql
        sp->custom_DiminishStatus = Spell::getDiminishingGroup(sp->getId());

        // various flight spells
        // these make vehicles and other charmed stuff fliable
        if (sp->activeIconID == 2158)
            sp->Attributes |= ATTRIBUTES_PASSIVE;


        //Name includes "" overwrites
        switch (sp->getId())
        {
        case 70908:
            sp->EffectImplicitTargetA[0] = EFF_TARGET_DYNAMIC_OBJECT;
            break;
            // Name includes "Winter's Chill"
            // Winter's Chill handled by frost school
            case 11180:     // Winter's Chill Rank 1
            case 12579:
            case 28592:     // Winter's Chill Rank 2
            case 28593:     // Winter's Chill Rank 3
            case 63094:
            {
                sp->School = SCHOOL_FROST;
            } break;

            // Name includes "Chain Heal"
            // more triggered spell ids are wrong. I think blizz is trying to outsmart us :S
            // Chain Heal all ranks %50 heal value (49 + 1)
            case 1064:
            case 10622:
            case 10623:
            case 14900:
            case 15799:
            case 16367:
            case 21899:
            case 23573:
            case 25422:
            case 25423:
            case 26122:
            case 30872:
            case 30873:
            case 33642:
            case 38322:
            case 38434:
            case 38435:
            case 41114:
            case 42027:
            case 42477:
            case 43527:
            case 43752:
            case 48894:
            case 54481:
            case 55437:
            case 55458:
            case 55459:
            case 55537:
            case 57232:
            case 59473:
            case 60167:
            case 61321:
            case 67226:
            case 67389:
            case 69923:
            case 70425:
            case 71120:
            case 75370:
            {
                sp->EffectDieSides[0] = 49;
            } break;

            default:
                break;
        }

        // Set default mechanics if we don't already have one
        if (!sp->MechanicsType)
        {
            //Set Silencing spells mechanic.
            if (sp->EffectApplyAuraName[0] == SPELL_AURA_MOD_SILENCE ||
                sp->EffectApplyAuraName[1] == SPELL_AURA_MOD_SILENCE ||
                sp->EffectApplyAuraName[2] == SPELL_AURA_MOD_SILENCE)
                sp->MechanicsType = MECHANIC_SILENCED;

            //Set Stunning spells mechanic.
            if (sp->EffectApplyAuraName[0] == SPELL_AURA_MOD_STUN ||
                sp->EffectApplyAuraName[1] == SPELL_AURA_MOD_STUN ||
                sp->EffectApplyAuraName[2] == SPELL_AURA_MOD_STUN)
                sp->MechanicsType = MECHANIC_STUNNED;

            //Set Fearing spells mechanic
            if (sp->EffectApplyAuraName[0] == SPELL_AURA_MOD_FEAR ||
                sp->EffectApplyAuraName[1] == SPELL_AURA_MOD_FEAR ||
                sp->EffectApplyAuraName[2] == SPELL_AURA_MOD_FEAR)
                sp->MechanicsType = MECHANIC_FLEEING;

            //Set Interrupted spells mech
            if (sp->Effect[0] == SPELL_EFFECT_INTERRUPT_CAST ||
                sp->Effect[1] == SPELL_EFFECT_INTERRUPT_CAST ||
                sp->Effect[2] == SPELL_EFFECT_INTERRUPT_CAST)
                sp->MechanicsType = MECHANIC_INTERRUPTED;
        }

        if (sp->custom_proc_interval > 0)      // if (sp->custom_proc_interval != 0)
            sp->procFlags |= PROC_REMOVEONUSE;

        //shaman - shock, has no spellgroup.very dangerous move !

        //mage - fireball. Only some of the spell has the flags

        switch (sp->getId())
        {
            // SPELL_HASH_SEAL_OF_COMMAND
            case 20375:     // Seal of Command - School/dmg_type
            case 20424:
            case 29385:
            case 33127:
            case 41469:
            case 42058:
            case 57769:
            case 57770:
            case 66004:
            case 68020:
            case 68021:
            case 68022:
            case 69403:
            {
                sp->School = SCHOOL_HOLY; //the procspells of the original seal of command have physical school instead of holy
                sp->Spell_Dmg_Type = SPELL_DMG_TYPE_MAGIC; //heh, crazy spell uses melee/ranged/magic dmg type for 1 spell. Now which one is correct ?
            } break;

            // SPELL_HASH_JUDGEMENT_OF_COMMAND
            case 20425:
            case 20467:
            case 29386:
            case 32778:
            case 33554:
            case 41368:
            case 41470:
            case 66005:
            case 68017:
            case 68018:
            case 68019:
            case 71551:
            {
                sp->Spell_Dmg_Type = SPELL_DMG_TYPE_MAGIC;
            } break;


            // SPELL_HASH_BLESSING_OF_PROTECTION
            case 41450:
            // SPELL_HASH_DIVINE_PROTECTION
            case 498:
            case 13007:
            case 27778:
            case 27779:
            // SPELL_HASH_DIVINE_SHIELD
            case 642:
            case 13874:
            case 29382:
            case 33581:
            case 40733:
            case 41367:
            case 54322:
            case 63148:
            case 66010:
            case 67251:
            case 71550:
            {
                sp->MechanicsType = MECHANIC_INVULNARABLE;
            } break;
            // SPELL_HASH_SHRED
            case 3252:
            case 5221:      // Shred Rank 1
            case 6800:      // Shred Rank 2
            case 8992:      // Shred Rank 3
            case 9829:      // Shred Rank 4
            case 9830:      // Shred Rank 5
            case 27001:     // Shred Rank 6
            case 27002:     // Shred Rank 7
            case 27555:
            case 48571:     // Shred Rank 8
            case 48572:     // Shred Rank 9
            case 49121:
            case 49165:
            case 61548:
            case 61549:
            // SPELL_HASH_BACKSTAB
            case 53:        // Backstab Rank 1
            case 2589:      // Backstab Rank 2
            case 2590:      // Backstab Rank 3
            case 2591:      // Backstab Rank 4
            case 7159:
            case 8721:      // Backstab Rank 5
            case 11279:     // Backstab Rank 6
            case 11280:     // Backstab Rank 7
            case 11281:     // Backstab Rank 8
            case 15582:
            case 15657:
            case 22416:
            case 25300:     // Backstab Rank 9
            case 26863:     // Backstab Rank 10
            case 30992:
            case 34614:
            case 37685:
            case 48656:     // Backstab Rank 11
            case 48657:     // Backstab Rank 12
            case 52540:
            case 58471:
            case 63754:
            case 71410:
            case 72427:
            // SPELL_HASH_AMBUSH
            case 8676:      // Ambush Rank 1
            case 8724:      // Ambush Rank 2
            case 8725:      // Ambush Rank 3
            case 11267:     // Ambush Rank 4
            case 11268:     // Ambush Rank 5
            case 11269:     // Ambush Rank 6
            case 24337:
            case 27441:     // Ambush Rank 7
            case 39668:
            case 39669:
            case 41390:
            case 48689:     // Ambush Rank 8
            case 48690:     // Ambush Rank 9
            case 48691:     // Ambush Rank 10
            case 56239:
            // SPELL_HASH_GARROTE
            case 703:       // Garrote Rank 1
            case 8631:      // Garrote Rank 2
            case 8632:      // Garrote Rank 3
            case 8633:      // Garrote Rank 4
            case 8818:      // Garrote Rank 4
            case 11289:     // Garrote Rank 5
            case 11290:     // Garrote Rank 6
            case 26839:     // Garrote Rank 7
            case 26884:     // Garrote Rank 8
            case 37066:
            case 48675:     // Garrote Rank 9
            case 48676:     // Garrote Rank 10
            // SPELL_HASH_RAVAGE
            case 3242:
            case 3446:
            case 6785:      // Ravage Rank 1
            case 6787:      // Ravage Rank 2
            case 8391:
            case 9866:      // Ravage Rank 3
            case 9867:      // Ravage Rank 4
            case 24213:
            case 24333:
            case 27005:     // Ravage Rank 5
            case 29906:
            case 33781:
            case 48578:     // Ravage Rank 6
            case 48579:     // Ravage Rank 7
            case 50518:     // Ravage Rank 1
            case 53558:     // Ravage Rank 2
            case 53559:     // Ravage Rank 3
            case 53560:     // Ravage Rank 4
            case 53561:     // Ravage Rank 5
            case 53562:     // Ravage Rank 6
            {
                // FIX ME: needs different flag check
                sp->FacingCasterFlags = SPELL_INFRONT_STATUS_REQUIRE_INBACK;
            } break;
        }
    }

    /////////////////////////////////////////////////////////////////
    //SPELL COEFFICIENT SETTINGS START
    //////////////////////////////////////////////////////////////////

    for (auto it = sSpellCustomizations.GetSpellInfoStore()->begin(); it != sSpellCustomizations.GetSpellInfoStore()->end(); ++it)
    {
        // get spellentry
        sp = sSpellCustomizations.GetSpellInfo(it->first);
        if (sp == nullptr)
            continue;

        //Setting Cast Time Coefficient
        auto spell_cast_time = sSpellCastTimesStore.LookupEntry(sp->CastingTimeIndex);
        float castaff = float(GetCastTime(spell_cast_time));
        if (castaff < 1500)
            castaff = 1500;
        else if (castaff > 7000)
            castaff = 7000;

        sp->casttime_coef = castaff / 3500;

        //Calculating fixed coeficients
        //Channeled spells
        if (sp->ChannelInterruptFlags != 0)
        {
            float Duration = float(GetDuration(sSpellDurationStore.LookupEntry(sp->DurationIndex)));
            if (Duration < 1500)
                Duration = 1500;
            else if (Duration > 7000)
                Duration = 7000;

            sp->fixed_hotdotcoef = (Duration / 3500.0f);

            if (sp->custom_spell_coef_flags & SPELL_FLAG_ADITIONAL_EFFECT)
                sp->fixed_hotdotcoef *= 0.95f;
            if (sp->custom_spell_coef_flags & SPELL_FLAG_AOE_SPELL)
                sp->fixed_hotdotcoef *= 0.5f;
        }

        //Standard spells
        else if ((sp->custom_spell_coef_flags & SPELL_FLAG_IS_DD_OR_DH_SPELL) && !(sp->custom_spell_coef_flags & SPELL_FLAG_IS_DOT_OR_HOT_SPELL))
        {
            sp->fixed_dddhcoef = sp->casttime_coef;
            if (sp->custom_spell_coef_flags & SPELL_FLAG_ADITIONAL_EFFECT)
                sp->fixed_dddhcoef *= 0.95f;
            if (sp->custom_spell_coef_flags & SPELL_FLAG_AOE_SPELL)
                sp->fixed_dddhcoef *= 0.5f;
        }

        //Over-time spells
        else if (!(sp->custom_spell_coef_flags & SPELL_FLAG_IS_DD_OR_DH_SPELL) && (sp->custom_spell_coef_flags & SPELL_FLAG_IS_DOT_OR_HOT_SPELL))
        {
            float Duration = float(GetDuration(sSpellDurationStore.LookupEntry(sp->DurationIndex)));
            sp->fixed_hotdotcoef = (Duration / 15000.0f);

            if (sp->custom_spell_coef_flags & SPELL_FLAG_ADITIONAL_EFFECT)
                sp->fixed_hotdotcoef *= 0.95f;
            if (sp->custom_spell_coef_flags & SPELL_FLAG_AOE_SPELL)
                sp->fixed_hotdotcoef *= 0.5f;

        }

        //Combined standard and over-time spells
        else if (sp->custom_spell_coef_flags & SPELL_FLAG_IS_DD_DH_DOT_SPELL)
        {
            float Duration = float(GetDuration(sSpellDurationStore.LookupEntry(sp->DurationIndex)));
            float Portion_to_Over_Time = (Duration / 15000.0f) / ((Duration / 15000.0f) + sp->casttime_coef);
            float Portion_to_Standard = 1.0f - Portion_to_Over_Time;

            sp->fixed_dddhcoef = sp->casttime_coef * Portion_to_Standard;
            sp->fixed_hotdotcoef = (Duration / 15000.0f) * Portion_to_Over_Time;

            if (sp->custom_spell_coef_flags & SPELL_FLAG_ADITIONAL_EFFECT)
            {
                sp->fixed_dddhcoef *= 0.95f;
                sp->fixed_hotdotcoef *= 0.95f;
            }
            if (sp->custom_spell_coef_flags & SPELL_FLAG_AOE_SPELL)
            {
                sp->fixed_dddhcoef *= 0.5f;
                sp->fixed_hotdotcoef *= 0.5f;
            }
        }

        // DankoDJ: This switch replaces the old NameHash overwrites
        switch (sp->getId())
        {
            //////////////////////////////////////////////////////////////////////////////////////////
            // SPELL_HASH_SHIELD_OF_RIGHTEOUSNESS
            case 53600:     // Shield of Righteousness Rank 1
            case 61411:     // Shield of Righteousness Rank 2
            {
                sp->School = SCHOOL_HOLY;
                sp->Effect[0] = SPELL_EFFECT_DUMMY;
                sp->Effect[1] = SPELL_EFFECT_NULL;          //hacks, handling it in Spell::SpellEffectSchoolDMG(uint32 i)
                sp->Effect[2] = SPELL_EFFECT_SCHOOL_DAMAGE; //hack
            } break;

            //////////////////////////////////////////////////////////////////////////////////////////
            // SPELL_HASH_CONSECRATION
            case 20116:     // Consecration Rank 2
            case 20922:     // Consecration Rank 3
            case 20923:     // Consecration Rank 4
            case 20924:     // Consecration Rank 5
            case 26573:     // Consecration Rank 1
            case 27173:     // Consecration Rank 6
            case 32773:
            case 33559:
            case 36946:
            case 37553:
            case 38385:
            case 41541:
            case 43429:
            case 48818:     // Consecration Rank 7
            case 48819:     // Consecration Rank 8
            case 57798:
            case 59998:
            case 69930:
            case 71122:
            {
                sp->School = SCHOOL_HOLY; //Consecration is a holy redirected spell.
                sp->Spell_Dmg_Type = SPELL_DMG_TYPE_MAGIC; //Speaks for itself.
            } break;

            //////////////////////////////////////////////////////////////////////////////////////////
            // SPELL_HASH_SEALS_OF_THE_PURE
#if VERSION_STRING != Cata
            case 20224:     // Seals of the Pure Rank 1
            case 20225:     // Seals of the Pure Rank 2
            case 20330:     // Seals of the Pure Rank 3
            case 20331:     // Seals of the Pure Rank 4
            case 20332:     // Seals of the Pure Rank 5
            {
                sp->EffectSpellClassMask[0][0] = 0x08000400;
                sp->EffectSpellClassMask[0][1] = 0x20000000;
                sp->EffectSpellClassMask[1][1] = 0x800;
            } break;
#endif

            //////////////////////////////////////////////////////////////////////////////////////////
            // SPELL_HASH_MEND_PET
            case 136:       // Mend Pet Rank 1
            case 3111:      // Mend Pet Rank 2
            case 3661:      // Mend Pet Rank 3
            case 3662:      // Mend Pet Rank 4
            case 13542:     // Mend Pet Rank 5
            case 13543:     // Mend Pet Rank 6
            case 13544:     // Mend Pet Rank 7
            case 27046:     // Mend Pet Rank 8
            case 33976:     // Mend Pet
            case 48989:     // Mend Pet Rank 9
            case 48990:     // Mend Pet Rank 10
            {
                sp->ChannelInterruptFlags = 0;
            } break;

            //////////////////////////////////////////////////////////////////////////////////////////
            // SPELL_HASH_GRACE (without ranks)
            case 47930:     // Grace
            {
                sp->rangeIndex = 4;
            } break;

            //////////////////////////////////////////////////////////////////////////////////////////
            // SPELL_HASH_FLAMETONGUE_ATTACK
            case 10444:     // Flametongue Attack
            case 65978:
            case 68109:
            case 68110:
            case 68111:
            {
                //sp->Effect[1] = SPELL_EFFECT_DUMMY;
                sp->AttributesExC |= ATTRIBUTESEXC_NO_DONE_BONUS;
            } break;

            //////////////////////////////////////////////////////////////////////////////////////////
            // SPELL_HASH_FLAMETONGUE_TOTEM
            case 8227:      // Flametongue Totem
            case 8249:
            case 10526:
            case 16387:
            case 25557:
            case 52109:
            case 52110:
            case 52111:
            case 52112:
            case 52113:
            case 58649:
            case 58651:
            case 58652:
            case 58654:
            case 58655:
            case 58656:
            {
                // Flametongue Totem passive target fix
                sp->EffectImplicitTargetA[0] = EFF_TARGET_SELF;
                sp->EffectImplicitTargetB[0] = 0;
                sp->EffectImplicitTargetA[1] = EFF_TARGET_SELF;
                sp->EffectImplicitTargetB[1] = 0;
            } break;

            //////////////////////////////////////////////////////////////////////////////////////////
            // SPELL_HASH_FROSTBRAND_ATTACK
            case 8034:      // Frostbrand Attack
            case 8037:
            case 10458:
            case 16352:
            case 16353:
            case 25501:
            case 38617:
            case 54609:
            case 58797:
            case 58798:
            case 58799:
            case 64186:
            {
                // Frostbrand Weapon - 10% spd coefficient
                sp->fixed_dddhcoef = 0.1f;
                // Attributes addition
                sp->AttributesExC |= ATTRIBUTESEXC_NO_DONE_BONUS;
            } break;

            //////////////////////////////////////////////////////////////////////////////////////////
            // SPELL_HASH_FIRE_NOVA
            case 8349:      // Fire Nova
            case 8502:
            case 8503:
            case 11306:
            case 11307:
            case 11969:
            case 11970:
            case 12470:
            case 16079:
            case 16635:
            case 17366:
            case 18432:
            case 20203:
            case 20602:
            case 23462:
            case 25535:
            case 25537:
            case 26073:
            case 30941:
            case 32167:
            case 33132:
            case 33775:
            case 37371:
            case 38728:
            case 43464:
            case 46551:
            case 61163:
            case 61650:
            case 61654:
            case 61655:
            case 68969:
            case 69667:
            case 78723:
            case 78724:
            {
                // Fire Nova - 0% spd coefficient
                sp->fixed_dddhcoef = 0.0f;
            } break;

            //////////////////////////////////////////////////////////////////////////////////////////
            // SPELL_HASH_FIRE_NOVA (ranks)
            case 1535:      //Fire Nova Rank 1
            {
                sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
                sp->EffectTriggerSpell[1] = 8349;
                // Fire Nova - 0% spd coefficient
                sp->fixed_dddhcoef = 0.0f;
            } break;
            case 8498:      //Fire Nova Rank 2
            {
                sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
                sp->EffectTriggerSpell[1] = 8502;
                // Fire Nova - 0% spd coefficient
                sp->fixed_dddhcoef = 0.0f;
            } break;
            case 8499:      //Fire Nova Rank 3
            {
                sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
                sp->EffectTriggerSpell[1] = 8503;
                // Fire Nova - 0% spd coefficient
                sp->fixed_dddhcoef = 0.0f;
            } break;
            case 11314:     //Fire Nova Rank 4
            {
                sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
                sp->EffectTriggerSpell[1] = 11306;
                // Fire Nova - 0% spd coefficient
                sp->fixed_dddhcoef = 0.0f;
            } break;
            case 11315:     //Fire Nova Rank 5
            {
                sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
                sp->EffectTriggerSpell[1] = 11307;
                // Fire Nova - 0% spd coefficient
                sp->fixed_dddhcoef = 0.0f;
            } break;
            case 25546:     //Fire Nova Rank 6
            {
                sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
                sp->EffectTriggerSpell[1] = 25535;
                // Fire Nova - 0% spd coefficient
                sp->fixed_dddhcoef = 0.0f;
            } break;
            case 25547:     //Fire Nova Rank 7
            {
                sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
                sp->EffectTriggerSpell[1] = 25537;
                // Fire Nova - 0% spd coefficient
                sp->fixed_dddhcoef = 0.0f;
            } break;
            case 61649:     //Fire Nova Rank 8
            {
                sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
                sp->EffectTriggerSpell[1] = 61650;
                // Fire Nova - 0% spd coefficient
                sp->fixed_dddhcoef = 0.0f;
            } break;
            case 61657:     //Fire Nova Rank 9
            {
                sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
                sp->EffectTriggerSpell[1] = 61654;
                // Fire Nova - 0% spd coefficient
                sp->fixed_dddhcoef = 0.0f;
            } break;

            //////////////////////////////////////////////////////////////////////////////////////////
            // SPELL_HASH_ATTACK
            case 3606:      // Attack
            case 6350:
            case 6351:
            case 6352:
            case 7389:
            case 10435:
            case 10436:
            case 15037:
            case 22048:
            case 25530:
            case 31992:
            case 32969:
            case 38296:
            case 38584:
            case 39592:
            case 39593:
            case 58700:
            case 58701:
            case 58702:
            case 65998:
            case 68106:
            case 68107:
            case 68108:
            case 68866:
            case 74413:
            case 75100:
            {
                // Searing Totem - 8% spd coefficient
                sp->fixed_dddhcoef = 0.08f;
            } break;

            //////////////////////////////////////////////////////////////////////////////////////////
            // SPELL_HASH_HEALING_STREAM
            case 5672:      // Healing Stream
            case 6371:      // Healing Stream
            case 6372:      // Healing Stream
            case 10460:     // Healing Stream
            case 10461:     // Healing Stream
            case 25566:     // Healing Stream
            case 58763:     // Healing Stream
            case 58764:     // Healing Stream
            case 58765:     // Healing Stream
            case 65994:     // Healing Stream
            case 68882:     // Healing Stream
            {
                // 8% healing coefficient
                sp->OTspell_coef_override = 0.08f;
            } break;

            //////////////////////////////////////////////////////////////////////////////////////////
            // SPELL_HASH_HEX
            case 11641:     // Hex
            case 16097:     // Hex
            case 16707:     // Hex
            case 16708:     // Hex
            case 16709:     // Hex
            case 17172:     // Hex
            case 18503:     // Hex
            case 22566:     // Hex
            case 24053:     // Hex
            case 29044:     // Hex
            case 36700:     // Hex
            case 40400:     // Hex
            case 46295:     // Hex
            case 51514:     // Hex
            case 53439:     // Hex
            case 66054:     // Hex
            {
                sp->AuraInterruptFlags |= AURA_INTERRUPT_ON_UNUSED2;
            } break;

            //////////////////////////////////////////////////////////////////////////////////////////
            // SPELL_HASH_DASH
            case 1850:      // Dash
            case 9821:      // Dash
            case 33357:     // Dash
            case 36589:     // Dash
            case 43317:     // Dash
            case 44029:     // Dash
            case 44531:     // Dash
            case 61684:     // Dash
            {
                // mask for FORM_CAT(1) = 1 << (1 - 1), which is 1
                sp->RequiredShapeShift = 1;
            } break;
            default:
                break;
        }
    }
    // END OF LOOP

    //Settings for special cases
    QueryResult* resultx = WorldDatabase.Query("SELECT * FROM spell_coef_override");
    if (resultx != NULL)
    {
        do
        {
            Field* f;
            f = resultx->Fetch();
            sp = sSpellCustomizations.GetSpellInfo(f[0].GetUInt32());
            if (sp != nullptr)
            {
                sp->Dspell_coef_override = f[2].GetFloat();
                sp->OTspell_coef_override = f[3].GetFloat();
            }
            else
                LOG_ERROR("Has nonexistent spell %u.", f[0].GetUInt32());
        }
        while (resultx->NextRow());
        delete resultx;
    }

    //Fully loaded coefficients, we must share channeled coefficient to its triggered spells
    for (auto it = sSpellCustomizations.GetSpellInfoStore()->begin(); it != sSpellCustomizations.GetSpellInfoStore()->end(); ++it)
    {
        // get spellentry
        sp = sSpellCustomizations.GetSpellInfo(it->first);
        if (sp == nullptr)
            continue;

        SpellInfo* spz;

        //Case SPELL_AURA_PERIODIC_TRIGGER_SPELL
        for (uint8 i = 0; i < 3; ++i)
        {
            if (sp->EffectApplyAuraName[i] == SPELL_AURA_PERIODIC_TRIGGER_SPELL)
            {
                spz = Spell::checkAndReturnSpellEntry(sp->EffectTriggerSpell[i]);
                if (spz != NULL)
                {
                    if (sp->Dspell_coef_override >= 0)
                        spz->Dspell_coef_override = sp->Dspell_coef_override;
                    else
                    {
                        //we must set bonus per tick on triggered spells now (i.e. Arcane Missiles)
                        if (sp->ChannelInterruptFlags != 0)
                        {
                            float Duration = float(GetDuration(sSpellDurationStore.LookupEntry(sp->DurationIndex)));
                            float amp = float(sp->EffectAmplitude[i]);
                            sp->fixed_dddhcoef = sp->fixed_hotdotcoef * amp / Duration;
                        }
                        spz->fixed_dddhcoef = sp->fixed_dddhcoef;
                    }

                    if (sp->OTspell_coef_override >= 0)
                        spz->OTspell_coef_override = sp->OTspell_coef_override;
                    else
                    {
                        //we must set bonus per tick on triggered spells now (i.e. Arcane Missiles)
                        if (sp->ChannelInterruptFlags != 0)
                        {
                            float Duration = float(GetDuration(sSpellDurationStore.LookupEntry(sp->DurationIndex)));
                            float amp = float(sp->EffectAmplitude[i]);
                            sp->fixed_hotdotcoef *= amp / Duration;
                        }
                        spz->fixed_hotdotcoef = sp->fixed_hotdotcoef;
                    }
                    break;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////
    //SPELL COEFFICIENT SETTINGS END
    /////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////
    // thrown - add a 1.6 second cooldown
    const static uint32 thrown_spells[] = { SPELL_RANGED_GENERAL, SPELL_RANGED_THROW, SPELL_RANGED_WAND, 26679, 29436, 37074, 41182, 41346, 0 };
    for (uint32 i = 0; thrown_spells[i] != 0; ++i)
    {
        sp = Spell::checkAndReturnSpellEntry(thrown_spells[i]);
        if (sp != nullptr && sp->RecoveryTime == 0 && sp->StartRecoveryTime == 0)
        {
            if (sp->getId() == SPELL_RANGED_GENERAL)
                sp->RecoveryTime = 500;    // cebernic: hunter general with 0.5s
            else
                sp->RecoveryTime = 1500; // 1.5cd
        }
    }

    ////////////////////////////////////////////////////////////
    // Wands
    sp = Spell::checkAndReturnSpellEntry(SPELL_RANGED_WAND);
    if (sp != nullptr)
        sp->Spell_Dmg_Type = SPELL_DMG_TYPE_RANGED;


    //////////////////////////////////////////////////////
    // CLASS-SPECIFIC SPELL FIXES                        //
    //////////////////////////////////////////////////////

    // Note: when applying spell hackfixes, please follow a template

    //////////////////////////////////////////
    // WARRIOR                                //
    //////////////////////////////////////////

    // Insert warrior spell fixes here

    ////////////////////////////////////////////////////////////
    // Arms

    // Juggernaut
    sp = Spell::checkAndReturnSpellEntry(65156);
    if (sp != nullptr)
        sp->AuraInterruptFlags = AURA_INTERRUPT_ON_CAST_SPELL;

    // Warrior - Overpower Rank 1
    sp = Spell::checkAndReturnSpellEntry(7384);
    if (sp != nullptr)
        sp->Attributes |= ATTRIBUTES_CANT_BE_DPB;
    // Warrior - Overpower Rank 2
    sp = Spell::checkAndReturnSpellEntry(7887);
    if (sp != nullptr)
        sp->Attributes |= ATTRIBUTES_CANT_BE_DPB;
    // Warrior - Overpower Rank 3
    sp = Spell::checkAndReturnSpellEntry(11584);
    if (sp != nullptr)
        sp->Attributes |= ATTRIBUTES_CANT_BE_DPB;
    // Warrior - Overpower Rank 4
    sp = Spell::checkAndReturnSpellEntry(11585);
    if (sp != nullptr)
        sp->Attributes |= ATTRIBUTES_CANT_BE_DPB;

    // Warrior - Tactical Mastery Rank 1
    sp = Spell::checkAndReturnSpellEntry(12295);
    if (sp != nullptr)
        sp->RequiredShapeShift = 0x00070000;
    // Warrior - Tactical Mastery Rank 2
    sp = Spell::checkAndReturnSpellEntry(12676);
    if (sp != nullptr)
        sp->RequiredShapeShift = 0x00070000;
    // Warrior - Tactical Mastery Rank 3
    sp = Spell::checkAndReturnSpellEntry(12677);
    if (sp != nullptr)
        sp->RequiredShapeShift = 0x00070000;

    // Warrior - Heroic Throw
    sp = Spell::checkAndReturnSpellEntry(57755);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_SCHOOL_DAMAGE;
    }

    // Warrior - Rend
    sp = Spell::checkAndReturnSpellEntry(772);
    if (sp != nullptr)
        sp->MechanicsType = MECHANIC_BLEEDING;
    sp = Spell::checkAndReturnSpellEntry(6546);
    if (sp != nullptr)
        sp->MechanicsType = MECHANIC_BLEEDING;
    sp = Spell::checkAndReturnSpellEntry(6547);
    if (sp != nullptr)
        sp->MechanicsType = MECHANIC_BLEEDING;
    sp = Spell::checkAndReturnSpellEntry(6548);
    if (sp != nullptr)
        sp->MechanicsType = MECHANIC_BLEEDING;
    sp = Spell::checkAndReturnSpellEntry(11572);
    if (sp != nullptr)
        sp->MechanicsType = MECHANIC_BLEEDING;
    sp = Spell::checkAndReturnSpellEntry(11573);
    if (sp != nullptr)
        sp->MechanicsType = MECHANIC_BLEEDING;
    sp = Spell::checkAndReturnSpellEntry(11574);
    if (sp != nullptr)
        sp->MechanicsType = MECHANIC_BLEEDING;
    sp = Spell::checkAndReturnSpellEntry(25208);
    if (sp != nullptr)
        sp->MechanicsType = MECHANIC_BLEEDING;

    ////////////////////////////////////////////////////////////
    // Fury

    // Warrior - Slam
    sp = Spell::checkAndReturnSpellEntry(1464);
    if (sp != nullptr)
        sp->Effect[0] = SPELL_EFFECT_SCHOOL_DAMAGE;

    sp = Spell::checkAndReturnSpellEntry(8820);
    if (sp != nullptr)
        sp->Effect[0] = SPELL_EFFECT_SCHOOL_DAMAGE;

    sp = Spell::checkAndReturnSpellEntry(11604);
    if (sp != nullptr)
        sp->Effect[0] = SPELL_EFFECT_SCHOOL_DAMAGE;

    sp = Spell::checkAndReturnSpellEntry(11605);
    if (sp != nullptr)
        sp->Effect[0] = SPELL_EFFECT_SCHOOL_DAMAGE;

    sp = Spell::checkAndReturnSpellEntry(25241);
    if (sp != nullptr)
        sp->Effect[0] = SPELL_EFFECT_SCHOOL_DAMAGE;

    sp = Spell::checkAndReturnSpellEntry(25242);
    if (sp != nullptr)
        sp->Effect[0] = SPELL_EFFECT_SCHOOL_DAMAGE;

    sp = Spell::checkAndReturnSpellEntry(47474);
    if (sp != nullptr)
        sp->Effect[0] = SPELL_EFFECT_SCHOOL_DAMAGE;

    sp = Spell::checkAndReturnSpellEntry(47475);
    if (sp != nullptr)
        sp->Effect[0] = SPELL_EFFECT_SCHOOL_DAMAGE;

    // Warrior - Bloodthirst new version is ok but old version is wrong from now on :(
    sp = Spell::checkAndReturnSpellEntry(23881);
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL; //cast on us, it is good
        sp->EffectTriggerSpell[1] = 23885; //evil , but this is good for us :D
    }
    sp = Spell::checkAndReturnSpellEntry(23892);
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 23886; //evil , but this is good for us :D  // DankoDJ: Is there a reason to trigger an non existing spell?
    }
    sp = Spell::checkAndReturnSpellEntry(23893);
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL; //
        sp->EffectTriggerSpell[1] = 23887; //evil , but this is good for us :D // DankoDJ: Is there a reason to trigger an non existing spell?
    }
    sp = Spell::checkAndReturnSpellEntry(23894);
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL; //
        sp->EffectTriggerSpell[1] = 23888; //evil , but this is good for us :D // DankoDJ: Is there a reason to trigger an non existing spell?
    }
    sp = Spell::checkAndReturnSpellEntry(25251);
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL; //aura
        sp->EffectTriggerSpell[1] = 25252; //evil , but this is good for us :D // DankoDJ: Is there a reason to trigger an non existing spell?
    }
    sp = Spell::checkAndReturnSpellEntry(30335);
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL; //aura
        sp->EffectTriggerSpell[1] = 30339; //evil , but this is good for us :D // DankoDJ: Is there a reason to trigger an non existing spell?
    }

    // Warrior - Berserker Rage
    sp = Spell::checkAndReturnSpellEntry(18499);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY;//Forcing a dummy aura, so we can add the missing 4th effect.
        sp->Effect[1] = SPELL_EFFECT_NULL;
        sp->Effect[2] = SPELL_EFFECT_NULL;
    }

    // Warrior - Heroic Fury
    sp = Spell::checkAndReturnSpellEntry(60970);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_DUMMY;
    }

    ////////////////////////////////////////////////////////////
    // Protection

    // Intervene  Ranger: stop attack
    sp = Spell::checkAndReturnSpellEntry(3411);
    if (sp != nullptr)
    {
        sp->Attributes |= ATTRIBUTES_STOP_ATTACK;
    }

    //////////////////////////////////////////
    // PALADIN                                //
    //////////////////////////////////////////

    // Insert paladin spell fixes here

    //Paladin - Seal of Command - Holy damage, but melee mechanics (crit damage, chance, etc)
    sp = Spell::checkAndReturnSpellEntry(20424);
    if (sp != nullptr)
        sp->custom_is_melee_spell = true;

    //Paladin - Hammer of the Righteous
    sp = Spell::checkAndReturnSpellEntry(53595);
    if (sp != nullptr)
    {
        sp->speed = 0;    //without, no damage is done
    }

    //Paladin - Seal of Martyr
    sp = Spell::checkAndReturnSpellEntry(53720);
    if (sp != nullptr)
    {
        sp->School = SCHOOL_HOLY;
    }
    //Paladin - seal of blood
    sp = Spell::checkAndReturnSpellEntry(31892);
    if (sp != nullptr)
    {
        sp->School = SCHOOL_HOLY;
    }
    sp = Spell::checkAndReturnSpellEntry(53719);
    if (sp != nullptr)
    {
        sp->School = SCHOOL_HOLY;
        sp->Spell_Dmg_Type = SPELL_DMG_TYPE_MAGIC;
    }
    sp = Spell::checkAndReturnSpellEntry(31893);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_PHYSICAL_ATTACK;
        sp->School = SCHOOL_HOLY;
        sp->Spell_Dmg_Type = SPELL_DMG_TYPE_MAGIC;
    }

    //Paladin - Divine Storm
    sp = Spell::checkAndReturnSpellEntry(53385);
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectRadiusIndex[0] = 43; //16 yards
        sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 54172;
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->procChance = 100;
        sp->MaxTargets = 4;
    }

    //Paladin - Judgements of the Wise
    sp = Spell::checkAndReturnSpellEntry(31930);
    if (sp != nullptr)
    {
        sp->SpellFamilyName = 0;
        sp->SpellGroupType[0] = 0;
        sp->SpellGroupType[1] = 0;
        sp->SpellGroupType[2] = 0;
    }

    sp = Spell::checkAndReturnSpellEntry(54180);
    if (sp != nullptr)
    {
        sp->SpellFamilyName = 0;
        sp->SpellGroupType[0] = 0;
        sp->SpellGroupType[1] = 0;
        sp->SpellGroupType[2] = 0;
    }

    //Paladin - Avenging Wrath marker - Is forced debuff
    sp = Spell::checkAndReturnSpellEntry(61987);
    if (sp != nullptr)
    {
        sp->Attributes = ATTRIBUTES_IGNORE_INVULNERABILITY;
    }

    //Paladin - Forbearance - Is forced debuff
    sp = Spell::checkAndReturnSpellEntry(25771);
    if (sp != nullptr)
    {
        sp->Attributes = ATTRIBUTES_IGNORE_INVULNERABILITY;
    }

    //Divine Protection
    sp = Spell::checkAndReturnSpellEntry(498);
    if (sp != nullptr)
        sp->targetAuraSpellNot = 25771;

    //Divine Shield
    sp = Spell::checkAndReturnSpellEntry(642);
    if (sp != nullptr)
        sp->targetAuraSpellNot = 25771;

    //Hand of Protection Rank 1
    sp = Spell::checkAndReturnSpellEntry(1022);
    if (sp != nullptr)
        sp->targetAuraSpellNot = 25771;

    //Hand of Protection Rank 2
    sp = Spell::checkAndReturnSpellEntry(5599);
    if (sp != nullptr)
        sp->targetAuraSpellNot = 25771;

    //Hand of Protection Rank 3
    sp = Spell::checkAndReturnSpellEntry(10278);
    if (sp != nullptr)
        sp->targetAuraSpellNot = 25771;

    //Paladin - Art of War
    sp = Spell::checkAndReturnSpellEntry(53486);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_DAMAGE_DONE;
    }
    sp = Spell::checkAndReturnSpellEntry(53489);
    if (sp != nullptr)
        sp->AuraInterruptFlags = AURA_INTERRUPT_ON_CAST_SPELL;

    sp = Spell::checkAndReturnSpellEntry(53488);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_DAMAGE_DONE;
    }
    sp = Spell::checkAndReturnSpellEntry(59578);
    if (sp != nullptr)
        sp->AuraInterruptFlags = AURA_INTERRUPT_ON_CAST_SPELL;

    //Paladin - Hammer of Justice - Interrupt effect
    sp = Spell::checkAndReturnSpellEntry(853);
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 32747;
    }
    sp = Spell::checkAndReturnSpellEntry(5588);
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 32747;
    }
    sp = Spell::checkAndReturnSpellEntry(5589);
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 32747;
    }
    sp = Spell::checkAndReturnSpellEntry(10308);
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 32747;
    }

    //////////////////////////////////////////
    // HUNTER                                //
    //////////////////////////////////////////

    // Insert hunter spell fixes here

    //Hunter - Bestial Wrath
    sp = Spell::checkAndReturnSpellEntry(19574);
    if (sp != nullptr)
        sp->EffectApplyAuraName[2] = SPELL_AURA_DUMMY;

    //Hunter - The Beast Within
    sp = Spell::checkAndReturnSpellEntry(34471);
    if (sp != nullptr)
        sp->EffectApplyAuraName[2] = SPELL_AURA_DUMMY;

    //Hunter - Go for the Throat
    sp = Spell::checkAndReturnSpellEntry(34952);
    if (sp != nullptr)
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
    sp = Spell::checkAndReturnSpellEntry(34953);
    if (sp != nullptr)
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;

    // Hunter - Spirit Bond
    sp = Spell::checkAndReturnSpellEntry(19578);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 19579;
    }
    sp = Spell::checkAndReturnSpellEntry(20895);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 24529;
    }
    sp = Spell::checkAndReturnSpellEntry(19579);
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_APPLY_AURA; //we should do the same for player too as we did for pet
        sp->EffectApplyAuraName[1] = sp->EffectApplyAuraName[0];
        sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
        sp->EffectBasePoints[1] = sp->EffectBasePoints[0];
        sp->EffectAmplitude[1] = sp->EffectAmplitude[0];
        sp->EffectDieSides[1] = sp->EffectDieSides[0];
    }
    sp = Spell::checkAndReturnSpellEntry(24529);
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_APPLY_AURA; //we should do the same for player too as we did for pet
        sp->EffectApplyAuraName[1] = sp->EffectApplyAuraName[0];
        sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
        sp->EffectBasePoints[1] = sp->EffectBasePoints[0];
        sp->EffectAmplitude[1] = sp->EffectAmplitude[0];
        sp->EffectDieSides[1] = sp->EffectDieSides[0];
    }

    //Hunter Silencing Shot
    sp = Spell::checkAndReturnSpellEntry(34490);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_SILENCE;
    }

    // Hunter - Ferocious Inspiration
    sp = Spell::checkAndReturnSpellEntry(34455);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
        sp->EffectTriggerSpell[0] = 34456;
        sp->procFlags = PROC_ON_CRIT_ATTACK | PROC_ON_SPELL_CRIT_HIT | static_cast<uint32>(PROC_TARGET_SELF); //maybe target master ?
        sp->Effect[1] = SPELL_EFFECT_NULL; //remove this
    }
    sp = Spell::checkAndReturnSpellEntry(34459);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
        sp->EffectTriggerSpell[0] = 34456;
        sp->procFlags = PROC_ON_CRIT_ATTACK | PROC_ON_SPELL_CRIT_HIT | static_cast<uint32>(PROC_TARGET_SELF);
        sp->Effect[1] = SPELL_EFFECT_NULL; //remove this
    }
    sp = Spell::checkAndReturnSpellEntry(34460);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
        sp->EffectTriggerSpell[0] = 34456;
        sp->procFlags = PROC_ON_CRIT_ATTACK | PROC_ON_SPELL_CRIT_HIT | static_cast<uint32>(PROC_TARGET_SELF);
        sp->Effect[1] = SPELL_EFFECT_NULL; //remove this
    }

    // Hunter - Focused Fire
    sp = Spell::checkAndReturnSpellEntry(35029);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 35060;
    }
    sp = Spell::checkAndReturnSpellEntry(35030);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 35061;
    }

    // Hunter - Thrill of the Hunt
    sp = Spell::checkAndReturnSpellEntry(34497);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_SPELL_CRIT_HIT | static_cast<uint32>(PROC_TARGET_SELF);
        sp->procChance = sp->EffectBasePoints[0] + 1;
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 34720;
    }
    sp = Spell::checkAndReturnSpellEntry(34498);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_SPELL_CRIT_HIT | static_cast<uint32>(PROC_TARGET_SELF);
        sp->procChance = sp->EffectBasePoints[0] + 1;
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 34720;
    }
    sp = Spell::checkAndReturnSpellEntry(34499);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_SPELL_CRIT_HIT | static_cast<uint32>(PROC_TARGET_SELF);
        sp->procChance = sp->EffectBasePoints[0] + 1;
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 34720;
    }

    //Hunter - Frenzy
    sp = Spell::checkAndReturnSpellEntry(19621);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 19615;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
        sp->procChance = sp->EffectBasePoints[0];
        sp->procFlags = PROC_ON_CRIT_ATTACK | static_cast<uint32>(PROC_TARGET_SELF);        //Zyres: moved from custom_c_is_flag
    }
    sp = Spell::checkAndReturnSpellEntry(19622);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 19615;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
        sp->procChance = sp->EffectBasePoints[0];
        sp->procFlags = PROC_ON_CRIT_ATTACK | static_cast<uint32>(PROC_TARGET_SELF);        //Zyres: moved from custom_c_is_flag
    }
    sp = Spell::checkAndReturnSpellEntry(19623);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 19615;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
        sp->procChance = sp->EffectBasePoints[0];
        sp->procFlags = PROC_ON_CRIT_ATTACK | static_cast<uint32>(PROC_TARGET_SELF);        //Zyres: moved from custom_c_is_flag
    }
    sp = Spell::checkAndReturnSpellEntry(19624);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 19615;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
        sp->procChance = sp->EffectBasePoints[0];
        sp->procFlags = PROC_ON_CRIT_ATTACK | static_cast<uint32>(PROC_TARGET_SELF);        //Zyres: moved from custom_c_is_flag
    }
    sp = Spell::checkAndReturnSpellEntry(19625);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 19615;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
        sp->procChance = sp->EffectBasePoints[0];
        sp->procFlags = PROC_ON_CRIT_ATTACK | static_cast<uint32>(PROC_TARGET_SELF);        //Zyres: moved from custom_c_is_flag
    }

    //Hunter : Pathfinding
    sp = Spell::checkAndReturnSpellEntry(19559);
    if (sp != nullptr)
    {
        sp->EffectMiscValue[0] = SMT_MISC_EFFECT;
    }
    sp = Spell::checkAndReturnSpellEntry(19560);
    if (sp != nullptr)
    {
        sp->EffectMiscValue[0] = SMT_MISC_EFFECT;
    }

    //Hunter : Rapid Killing - might need to add honor trigger too here. I'm guessing you receive Xp too so I'm avoiding double proc
    sp = Spell::checkAndReturnSpellEntry(34948);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_GAIN_EXPIERIENCE | static_cast<uint32>(PROC_TARGET_SELF);
    }
    sp = Spell::checkAndReturnSpellEntry(34949);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_GAIN_EXPIERIENCE | static_cast<uint32>(PROC_TARGET_SELF);
    }

    /* Zyres: Same procFlags are already in the dbcs!
    //Hunter : Entrapment
    sp = checkAndReturnSpellEntry(19184);
    if (sp != nullptr)
        sp->procFlags = PROC_ON_TRAP_TRIGGER;
    sp = checkAndReturnSpellEntry(19387);
    if (sp != nullptr)
        sp->procFlags = PROC_ON_TRAP_TRIGGER;
    sp = checkAndReturnSpellEntry(19388);
    if (sp != nullptr)
        sp->procFlags = PROC_ON_TRAP_TRIGGER;*/

    // Feed pet
    sp = Spell::checkAndReturnSpellEntry(6991);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[0] = 0;
    }

    //\todo 16/03/08 Zyres: sql
    // MesoX: Serendipity http://www.wowhead.com/?spell=63730
    sp = Spell::checkAndReturnSpellEntry(63730);   // Rank 1
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->custom_ProcOnNameHash[0] = SPELL_HASH_BINDING_HEAL;
        sp->custom_ProcOnNameHash[1] = SPELL_HASH_FLASH_HEAL;
    }
    sp = Spell::checkAndReturnSpellEntry(63733);   // Rank 2
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->custom_ProcOnNameHash[0] = SPELL_HASH_BINDING_HEAL;
        sp->custom_ProcOnNameHash[1] = SPELL_HASH_FLASH_HEAL;
    }
    sp = Spell::checkAndReturnSpellEntry(63737);   // Rank 3
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->custom_ProcOnNameHash[0] = SPELL_HASH_BINDING_HEAL;
        sp->custom_ProcOnNameHash[1] = SPELL_HASH_FLASH_HEAL;
    }


    //////////////////////////////////////////
    // ROGUE                                //
    //////////////////////////////////////////

    // Insert rogue spell fixes here

    // Garrote - this is used?
    sp = Spell::checkAndReturnSpellEntry(37066);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_ENEMY;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_SINGLE_ENEMY;
    }

    //rogue - Camouflage.
    sp = Spell::checkAndReturnSpellEntry(13975);
    if (sp != nullptr)
    {
        sp->EffectMiscValue[0] = SMT_MISC_EFFECT;
    }
    sp = Spell::checkAndReturnSpellEntry(14062);
    if (sp != nullptr)
    {
        sp->EffectMiscValue[0] = SMT_MISC_EFFECT;
    }
    sp = Spell::checkAndReturnSpellEntry(14063);
    if (sp != nullptr)
    {
        sp->EffectMiscValue[0] = SMT_MISC_EFFECT;
    }

    //rogue - Vanish : Second Trigger Spell
    sp = Spell::checkAndReturnSpellEntry(18461);
    if (sp != nullptr)
        sp->AttributesEx |= ATTRIBUTESEX_NOT_BREAK_STEALTH;

    // rogue - Blind (Make it able to miss!)
    sp = Spell::checkAndReturnSpellEntry(2094);
    if (sp != nullptr)
    {
        sp->Spell_Dmg_Type = SPELL_DMG_TYPE_RANGED;
        sp->custom_is_ranged_spell = true;
    }

    //rogue - Shadowstep
    sp = Spell::checkAndReturnSpellEntry(36563);
    if (sp != nullptr)
    {
        sp->EffectMiscValue[2] = SMT_MISC_EFFECT;
    }
    // Still related to shadowstep - prevent the trigger spells from breaking stealth.
    sp = Spell::checkAndReturnSpellEntry(44373);
    if (sp != nullptr)
        sp->AttributesEx |= ATTRIBUTESEX_NOT_BREAK_STEALTH;
    sp = Spell::checkAndReturnSpellEntry(36563);
    if (sp != nullptr)
        sp->AttributesEx |= ATTRIBUTESEX_NOT_BREAK_STEALTH;
    sp = Spell::checkAndReturnSpellEntry(36554);
    if (sp != nullptr)
        sp->AttributesEx |= ATTRIBUTESEX_NOT_BREAK_STEALTH;

    //garrot
    sp = Spell::checkAndReturnSpellEntry(703);
    if (sp != nullptr)
        sp->MechanicsType = MECHANIC_BLEEDING;
    sp = Spell::checkAndReturnSpellEntry(8631);
    if (sp != nullptr)
        sp->MechanicsType = MECHANIC_BLEEDING;
    sp = Spell::checkAndReturnSpellEntry(8632);
    if (sp != nullptr)
        sp->MechanicsType = MECHANIC_BLEEDING;
    sp = Spell::checkAndReturnSpellEntry(8633);
    if (sp != nullptr)
        sp->MechanicsType = MECHANIC_BLEEDING;
    sp = Spell::checkAndReturnSpellEntry(11289);
    if (sp != nullptr)
        sp->MechanicsType = MECHANIC_BLEEDING;
    sp = Spell::checkAndReturnSpellEntry(11290);
    if (sp != nullptr)
        sp->MechanicsType = MECHANIC_BLEEDING;
    sp = Spell::checkAndReturnSpellEntry(26839);
    if (sp != nullptr)
        sp->MechanicsType = MECHANIC_BLEEDING;
    sp = Spell::checkAndReturnSpellEntry(26884);
    if (sp != nullptr)
        sp->MechanicsType = MECHANIC_BLEEDING;

    //rupture
    sp = Spell::checkAndReturnSpellEntry(1943);
    if (sp != nullptr)
        sp->MechanicsType = MECHANIC_BLEEDING;
    sp = Spell::checkAndReturnSpellEntry(8639);
    if (sp != nullptr)
        sp->MechanicsType = MECHANIC_BLEEDING;
    sp = Spell::checkAndReturnSpellEntry(8640);
    if (sp != nullptr)
        sp->MechanicsType = MECHANIC_BLEEDING;
    sp = Spell::checkAndReturnSpellEntry(11273);
    if (sp != nullptr)
        sp->MechanicsType = MECHANIC_BLEEDING;
    sp = Spell::checkAndReturnSpellEntry(11274);
    if (sp != nullptr)
        sp->MechanicsType = MECHANIC_BLEEDING;
    sp = Spell::checkAndReturnSpellEntry(11275);
    if (sp != nullptr)
        sp->MechanicsType = MECHANIC_BLEEDING;
    sp = Spell::checkAndReturnSpellEntry(26867);
    if (sp != nullptr)
        sp->MechanicsType = MECHANIC_BLEEDING;

    //Rogue - Killing Spree Stealth fix
    sp = Spell::checkAndReturnSpellEntry(51690);
    if (sp != nullptr)
        sp->AttributesEx |= ATTRIBUTESEX_NOT_BREAK_STEALTH;


    //////////////////////////////////////////
    // PRIEST                                //
    //////////////////////////////////////////

    // Insert priest spell fixes here

    // Prayer of mending
    sp = Spell::checkAndReturnSpellEntry(41635);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_FRIEND;
    }
    sp = Spell::checkAndReturnSpellEntry(48110);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_FRIEND;
    }
    sp = Spell::checkAndReturnSpellEntry(48111);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_FRIEND;
    }
    sp = Spell::checkAndReturnSpellEntry(33110);
    if (sp != nullptr)
        sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_FRIEND;

    // Vampiric Embrace heal spell
    sp = Spell::checkAndReturnSpellEntry(15290);
    if (sp != nullptr)
    {
        sp->EffectBasePoints[0] = 2;
        sp->EffectBasePoints[1] = 14;
    }

    // Improved Mind Blast
    sp = Spell::checkAndReturnSpellEntry(15273);   //rank 1
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[1] = SPELL_AURA_DUMMY;
    }
    sp = Spell::checkAndReturnSpellEntry(15312);   //rank 2
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[1] = SPELL_AURA_DUMMY;
    }
    sp = Spell::checkAndReturnSpellEntry(15313);   //rank 3
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[1] = SPELL_AURA_DUMMY;
    }
    sp = Spell::checkAndReturnSpellEntry(15314);   //rank 4
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[1] = SPELL_AURA_DUMMY;
    }
    sp = Spell::checkAndReturnSpellEntry(15316);   //rank 5
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[1] = SPELL_AURA_DUMMY;
    }

    // Body and soul - fix duration of cleanse poison
    sp = Spell::checkAndReturnSpellEntry(64134);
    if (sp != nullptr)
        sp->DurationIndex = 29;

    // Spirit of Redemption - required spells can be casted while dead
    sp = Spell::checkAndReturnSpellEntry(27795);   // This is casted by shape shift
    if (sp != nullptr)
        sp->AttributesExC |= ATTRIBUTESEXC_CAN_PERSIST_AND_CASTED_WHILE_DEAD;
    sp = Spell::checkAndReturnSpellEntry(27792);   // This is casted by Apply Aura: Spirit of Redemption
    if (sp != nullptr)
        sp->AttributesExC |= ATTRIBUTESEXC_CAN_PERSIST_AND_CASTED_WHILE_DEAD;

    //Priest - Wand Specialization
    sp = Spell::checkAndReturnSpellEntry(14524);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
        sp->EffectMiscValue[0] = SMT_MISC_EFFECT;
    }
    sp = Spell::checkAndReturnSpellEntry(14525);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
        sp->EffectMiscValue[0] = SMT_MISC_EFFECT;
    }
    sp = Spell::checkAndReturnSpellEntry(14526);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
        sp->EffectMiscValue[0] = SMT_MISC_EFFECT;
    }
    sp = Spell::checkAndReturnSpellEntry(14527);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
        sp->EffectMiscValue[0] = SMT_MISC_EFFECT;
    }
    sp = Spell::checkAndReturnSpellEntry(14528);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
        sp->EffectMiscValue[0] = SMT_MISC_EFFECT;
    }

    //Priest - Inspiration proc spell
    sp = Spell::checkAndReturnSpellEntry(14893);
    if (sp != nullptr)
        sp->rangeIndex = 4;
    sp = Spell::checkAndReturnSpellEntry(15357);
    if (sp != nullptr)
        sp->rangeIndex = 4;
    sp = Spell::checkAndReturnSpellEntry(15359);
    if (sp != nullptr)
        sp->rangeIndex = 4;

    //priest - surge of light
    sp = Spell::checkAndReturnSpellEntry(33151);
    if (sp != nullptr)
    {
        sp->AuraInterruptFlags = AURA_INTERRUPT_ON_CAST_SPELL;
    }
    // priest - Reflective Shield
    sp = Spell::checkAndReturnSpellEntry(33201);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_ABSORB;
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 33619; //!! WRONG spell, we will make direct dmg here
    }
    sp = Spell::checkAndReturnSpellEntry(33202);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_ABSORB;
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 33619; //!! WRONG spell, we will make direct dmg here
    }
    // Weakened Soul - Is debuff
    sp = Spell::checkAndReturnSpellEntry(6788);
    if (sp != nullptr)
    {
        sp->Attributes = ATTRIBUTES_IGNORE_INVULNERABILITY;
    }

    // Penance
    sp = Spell::checkAndReturnSpellEntry(47540);
    if (sp != nullptr)
    {
        sp->DurationIndex = 566; // Change to instant cast as script will cast the real channeled spell.
        sp->ChannelInterruptFlags = 0; // Remove channeling behavior.
    }

    sp = Spell::checkAndReturnSpellEntry(53005);
    if (sp != nullptr)
    {
        sp->DurationIndex = 566;
        sp->ChannelInterruptFlags = 0;
    }

    sp = Spell::checkAndReturnSpellEntry(53006);
    if (sp != nullptr)
    {
        sp->DurationIndex = 566;
        sp->ChannelInterruptFlags = 0;
    }

    sp = Spell::checkAndReturnSpellEntry(53007);
    if (sp != nullptr)
    {
        sp->DurationIndex = 566;
        sp->ChannelInterruptFlags = 0;
    }

    // Penance triggered healing spells have wrong targets.
    sp = Spell::checkAndReturnSpellEntry(47750);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_FRIEND;
    }

    sp = Spell::checkAndReturnSpellEntry(52983);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_FRIEND;
    }

    sp = Spell::checkAndReturnSpellEntry(52984);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_FRIEND;
    }

    sp = Spell::checkAndReturnSpellEntry(52985);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_FRIEND;
    }


    //////////////////////////////////////////
    // SHAMAN                                //
    //////////////////////////////////////////

    // Insert shaman spell fixes here
    //shaman - Healing Way
    sp = Spell::checkAndReturnSpellEntry(29202);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;     // DankoDJ: No triggered Spell! We override SPELL_AURA_ADD_PCT_MODIFIER with this crap?
    }
    sp = Spell::checkAndReturnSpellEntry(29205);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;     // DankoDJ: No triggered Spell! We override SPELL_AURA_ADD_PCT_MODIFIER with this crap?
    }
    sp = Spell::checkAndReturnSpellEntry(29206);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;     // DankoDJ: No triggered Spell! We override SPELL_AURA_ADD_PCT_MODIFIER with this crap?
    }

    // Elemental Mastery
    sp = Spell::checkAndReturnSpellEntry(16166);
    if (sp != nullptr)
    {
        sp->EffectMiscValue[0] = SMT_CRITICAL;
        sp->EffectMiscValue[1] = SMT_COST;
        // sp->AuraInterruptFlags = AURA_INTERRUPT_ON_AFTER_CAST_SPELL;
    }

    ////////////////////////////////////////////////////////////
    // Shamanistic Rage
    SpellInfo*  parentsp = Spell::checkAndReturnSpellEntry(30823);
    SpellInfo* triggersp = Spell::checkAndReturnSpellEntry(30824);
    if (parentsp != nullptr && triggersp != nullptr)
        triggersp->EffectBasePoints[0] = parentsp->EffectBasePoints[0];

    //summon only 1 elemental totem
    sp = Spell::checkAndReturnSpellEntry(2894);
    if (sp != nullptr)
        sp->EffectImplicitTargetA[0] = EFF_TARGET_TOTEM_FIRE; //remove this targeting. it is enough to get 1 target

    //summon only 1 elemental totem
    sp = Spell::checkAndReturnSpellEntry(2062);
    if (sp != nullptr)
        sp->EffectImplicitTargetA[0] = EFF_TARGET_TOTEM_EARTH; //remove this targeting. it is enough to get 1 target


    ////////////////////////////////////////////////////////////
    // Bloodlust
    //Bloodlust
    sp = Spell::checkAndReturnSpellEntry(2825);
    if (sp != nullptr)
        sp->casterAuraSpellNot = 57724; //sated debuff

    // Sated - is debuff
    sp = Spell::checkAndReturnSpellEntry(57724);
    if (sp != nullptr)
    {
        sp->Attributes = ATTRIBUTES_IGNORE_INVULNERABILITY;
    }

    ////////////////////////////////////////////////////////////
    // Heroism
    //Heroism
    sp = Spell::checkAndReturnSpellEntry(32182);
    if (sp != nullptr)
        sp->casterAuraSpellNot = 57723; //sated debuff

    // Sated - is debuff
    sp = Spell::checkAndReturnSpellEntry(57723);
    if (sp != nullptr)
    {
        sp->Attributes = ATTRIBUTES_IGNORE_INVULNERABILITY;
    }

    ////////////////////////////////////////////////////////////
    // Purge
    sp = Spell::checkAndReturnSpellEntry(370);
    if (sp != nullptr)
        sp->DispelType = DISPEL_MAGIC;
    sp = Spell::checkAndReturnSpellEntry(8012);
    if (sp != nullptr)
        sp->DispelType = DISPEL_MAGIC;
    sp = Spell::checkAndReturnSpellEntry(27626);
    if (sp != nullptr)
        sp->DispelType = DISPEL_MAGIC;
    sp = Spell::checkAndReturnSpellEntry(33625);
    if (sp != nullptr)
        sp->DispelType = DISPEL_MAGIC;

    //Shaman - Shamanistic Focus
    // needs to be fixed (doesn't need to proc, it now just reduces mana cost always by %)
    sp = Spell::checkAndReturnSpellEntry(43338);
    if (sp != nullptr)
    {
        sp->EffectTriggerSpell[0] = 43339;
    }

    sp = Spell::checkAndReturnSpellEntry(43339);
    if (sp != nullptr)
    {
        sp->EffectMiscValue[0] = SMT_COST;
    }

    //shaman - Improved Chain Heal
    sp = Spell::checkAndReturnSpellEntry(30873);
    if (sp != nullptr)
    {
        sp->EffectDieSides[0] = 0;
    }
    sp = Spell::checkAndReturnSpellEntry(30872);
    if (sp != nullptr)
    {
        sp->EffectDieSides[0] = 0;
    }

    //shaman - Improved Weapon Totems
    sp = Spell::checkAndReturnSpellEntry(29193);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
        sp->EffectApplyAuraName[1] = SPELL_AURA_ADD_PCT_MODIFIER;
        sp->EffectMiscValue[0] = SMT_MISC_EFFECT;
        sp->EffectMiscValue[1] = SMT_MISC_EFFECT;
    }
    sp = Spell::checkAndReturnSpellEntry(29192);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
        sp->EffectApplyAuraName[1] = SPELL_AURA_ADD_PCT_MODIFIER;
        sp->EffectMiscValue[0] = SMT_MISC_EFFECT;
        sp->EffectMiscValue[1] = SMT_MISC_EFFECT;
    }

    // Shaman - Improved Fire Totems
    sp = Spell::checkAndReturnSpellEntry(16544);
    if (sp != nullptr)
    {
        sp->EffectMiscValue[0] = SMT_DURATION;
    }
    sp = Spell::checkAndReturnSpellEntry(16086);
    if (sp != nullptr)
    {
        sp->EffectMiscValue[0] = SMT_DURATION;
    }

    //shaman - Elemental Weapons
    sp = Spell::checkAndReturnSpellEntry(29080);
    if (sp != nullptr)
    {
        sp->EffectMiscValue[1] = SMT_DAMAGE_DONE;
        sp->EffectMiscValue[2] = SMT_DAMAGE_DONE;
    }
    sp = Spell::checkAndReturnSpellEntry(29079);
    if (sp != nullptr)
    {
        sp->EffectMiscValue[1] = SMT_DAMAGE_DONE;
        sp->EffectMiscValue[2] = SMT_DAMAGE_DONE;
    }
    sp = Spell::checkAndReturnSpellEntry(16266);
    if (sp != nullptr)
    {
        sp->EffectMiscValue[1] = SMT_DAMAGE_DONE;
        sp->EffectMiscValue[2] = SMT_DAMAGE_DONE;
    }

    // Magma Totem - 0% spd coefficient
    sp = Spell::checkAndReturnSpellEntry(25550);
    if (sp != nullptr)
        sp->fixed_dddhcoef = 0.0f;
    sp = Spell::checkAndReturnSpellEntry(10581);
    if (sp != nullptr)
        sp->fixed_dddhcoef = 0.0f;
    sp = Spell::checkAndReturnSpellEntry(10580);
    if (sp != nullptr)
        sp->fixed_dddhcoef = 0.0f;
    sp = Spell::checkAndReturnSpellEntry(10579);
    if (sp != nullptr)
        sp->fixed_dddhcoef = 0.0f;
    sp = Spell::checkAndReturnSpellEntry(8187);
    if (sp != nullptr)
        sp->fixed_dddhcoef = 0.0f;

    ////////////////////////////////////////////////////////////
    //  Unleashed Rage - LordLeeCH
    sp = Spell::checkAndReturnSpellEntry(30802);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CRIT_ATTACK;
        sp->Effect[0] = SPELL_EFFECT_APPLY_GROUP_AREA_AURA;
    }
    sp = Spell::checkAndReturnSpellEntry(30808);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CRIT_ATTACK;
        sp->Effect[0] = SPELL_EFFECT_APPLY_GROUP_AREA_AURA;
    }
    sp = Spell::checkAndReturnSpellEntry(30809);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CRIT_ATTACK;
        sp->Effect[0] = SPELL_EFFECT_APPLY_GROUP_AREA_AURA;
    }

    ////////////////////////////////////////////////////////////
    // Ancestral healing proc spell
    sp = Spell::checkAndReturnSpellEntry(16177);
    if (sp != nullptr)
        sp->rangeIndex = 4;
    sp = Spell::checkAndReturnSpellEntry(16236);
    if (sp != nullptr)
        sp->rangeIndex = 4;
    sp = Spell::checkAndReturnSpellEntry(16237);
    if (sp != nullptr)
        sp->rangeIndex = 4;

    sp = Spell::checkAndReturnSpellEntry(20608);   //Reincarnation
    if (sp != nullptr)
    {
        for (uint8 i = 0; i < 8; ++i)
        {
            if (sp->Reagent[i])
            {
                sp->Reagent[i] = 0;
                sp->ReagentCount[i] = 0;
            }
        }
    }

    //////////////////////////////////////////
    // SHAMAN WRATH OF AIR TOTEM            //
    //////////////////////////////////////////
    sp = Spell::checkAndReturnSpellEntry(2895);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_SELF;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_SELF;
        sp->EffectImplicitTargetA[2] = 0;
        sp->EffectImplicitTargetB[0] = 0;
        sp->EffectImplicitTargetB[1] = 0;
        sp->EffectImplicitTargetB[2] = 0;
    }

    // Rogue - Master of Subtlety
    sp = Spell::checkAndReturnSpellEntry(31665);
    if (sp != nullptr)
        sp->AttributesEx |= ATTRIBUTESEX_NOT_BREAK_STEALTH;

    //////////////////////////////////////////
    // MAGE                                    //
    //////////////////////////////////////////

    // Insert mage spell fixes here

    // Brain Freeze rank 1
    sp = Spell::checkAndReturnSpellEntry(44546);
    if (sp != nullptr)
        sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY;

    // Brain Freeze rank 2
    sp = Spell::checkAndReturnSpellEntry(44548);
    if (sp != nullptr)
        sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY;

    // Brain Freeze rank 3
    sp = Spell::checkAndReturnSpellEntry(44549);
    if (sp != nullptr)
        sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY;

    // Fingers of Frost rank 1
    sp = Spell::checkAndReturnSpellEntry(44543);
    if (sp != nullptr)
        sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY;


    // Fingers of Frost rank 2
    sp = Spell::checkAndReturnSpellEntry(44545);
    if (sp != nullptr)
        sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY;


    //Mage - Spell Power
    sp = Spell::checkAndReturnSpellEntry(35578);
    if (sp != nullptr)
    {
        sp->EffectMiscValue[0] = SMT_CRITICAL_DAMAGE;
        sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
    }
    sp = Spell::checkAndReturnSpellEntry(35581);
    if (sp != nullptr)
    {
        sp->EffectMiscValue[0] = SMT_CRITICAL_DAMAGE;
        sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
    }

    //Mage - Elemental Precision
    sp = Spell::checkAndReturnSpellEntry(29438);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
        sp->EffectMiscValue[0] = SMT_COST;
    }
    sp = Spell::checkAndReturnSpellEntry(29439);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
        sp->EffectMiscValue[0] = SMT_COST;
    }
    sp = Spell::checkAndReturnSpellEntry(29440);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
        sp->EffectMiscValue[0] = SMT_COST;
    }

    //Mage - Arcane Blast
    sp = Spell::checkAndReturnSpellEntry(30451);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->procFlags = PROC_ON_CAST_SPECIFIC_SPELL;
        sp->custom_ProcOnNameHash[1] = SPELL_HASH_ARCANE_BLAST;
    }

    // Arcane Blast
    sp = Spell::checkAndReturnSpellEntry(42894);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->procFlags = PROC_ON_CAST_SPECIFIC_SPELL;
        sp->custom_ProcOnNameHash[1] = SPELL_HASH_ARCANE_BLAST;
    }

    sp = Spell::checkAndReturnSpellEntry(42896);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->procFlags = PROC_ON_CAST_SPECIFIC_SPELL;
    }

    sp = Spell::checkAndReturnSpellEntry(42897);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->procFlags = PROC_ON_CAST_SPECIFIC_SPELL;
    }

    //mage : Empowered Arcane Missiles
    //heh B thinks he is smart by adding this to description ? If it doesn't work std then it still needs to made by hand
    sp = Spell::checkAndReturnSpellEntry(31579);
    if (sp != nullptr)
    {
        sp->EffectBasePoints[0] = 5 * (sp->EffectBasePoints[0] + 1);
    }
    sp = Spell::checkAndReturnSpellEntry(31582);
    if (sp != nullptr)
    {
        sp->EffectBasePoints[0] = 5 * (sp->EffectBasePoints[0] + 1);
    }
    sp = Spell::checkAndReturnSpellEntry(31583);
    if (sp != nullptr)
    {
        sp->EffectBasePoints[0] = 5 * (sp->EffectBasePoints[0] + 1);
    }

    // cebernic: not for self?
    // impact
    sp = Spell::checkAndReturnSpellEntry(12355);
    if (sp != nullptr)
    {
        // passive rank: 11103, 12357, 12358 ,12359,12360 :D
        sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM | PROC_ON_SPELL_CRIT_HIT | PROC_ON_SPELL_HIT;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_ALL_ENEMIES_AROUND_CASTER;
        sp->EffectImplicitTargetB[0] = EFF_TARGET_ALL_ENEMIES_AROUND_CASTER;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_ALL_ENEMIES_AROUND_CASTER;
        sp->EffectImplicitTargetB[1] = EFF_TARGET_ALL_ENEMIES_AROUND_CASTER;
        sp->EffectImplicitTargetA[2] = EFF_TARGET_ALL_ENEMIES_AROUND_CASTER;
        sp->EffectImplicitTargetB[2] = EFF_TARGET_ALL_ENEMIES_AROUND_CASTER;
    }

    //Mage - Invisibility
    sp = Spell::checkAndReturnSpellEntry(66);
    if (sp != nullptr)
    {
        sp->AuraInterruptFlags |= AURA_INTERRUPT_ON_CAST_SPELL;
        sp->Effect[1] = SPELL_EFFECT_NULL;
        sp->EffectApplyAuraName[2] = SPELL_AURA_PERIODIC_TRIGGER_SPELL;
        sp->Effect[2] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectAmplitude[2] = 3000;
        sp->EffectImplicitTargetA[2] = EFF_TARGET_SELF;
        //sp->EffectBaseDice[2] = 1;
        sp->EffectDieSides[2] = 1;
        sp->EffectTriggerSpell[2] = 32612;
        sp->EffectBasePoints[2] = -1;
    }

    //Invisibility triggered spell, should be removed on cast
    sp = Spell::checkAndReturnSpellEntry(32612);
    if (sp != nullptr)
    {
        sp->AuraInterruptFlags |= AURA_INTERRUPT_ON_CAST_SPELL;
    }

    //Arcane Potency procs
    sp = Spell::checkAndReturnSpellEntry(57529);
    if (sp != nullptr)
    {
        sp->procFlags = 0;
        // sp->custom_RankNumber = 100;            DankoDJ: Why?
        sp->AuraInterruptFlags = 0;
    }

    sp = Spell::checkAndReturnSpellEntry(57531);
    if (sp != nullptr)
    {
        sp->procFlags = 0;
        // sp->custom_RankNumber = 101;            DankoDJ: Why?
        sp->AuraInterruptFlags = 0;
    }

    //Hot Streak proc
    sp = Spell::checkAndReturnSpellEntry(48108);
    if (sp != nullptr)
    {
        sp->AuraInterruptFlags |= AURA_INTERRUPT_ON_CAST_SPELL;
    }

    //Ice Lances
    sp = Spell::checkAndReturnSpellEntry(42914);
    if (sp != nullptr)
        sp->Dspell_coef_override = 0.1429f;

    sp = Spell::checkAndReturnSpellEntry(42913);
    if (sp != nullptr)
        sp->Dspell_coef_override = 0.1429f;

    sp = Spell::checkAndReturnSpellEntry(30455);
    if (sp != nullptr)
        sp->Dspell_coef_override = 0.1429f;

    // Frostfire Bolts
    sp = Spell::checkAndReturnSpellEntry(47610);
    if (sp != nullptr)
        sp->fixed_dddhcoef = 0.8571f;

    sp = Spell::checkAndReturnSpellEntry(44614);
    if (sp != nullptr)
        sp->fixed_dddhcoef = 0.8571f;


    //mage - Combustion
    sp = Spell::checkAndReturnSpellEntry(11129);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_NULL;
        sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 28682;
        sp->procFlags = PROC_ON_SPELL_HIT | PROC_ON_SPELL_CRIT_HIT | static_cast<uint32>(PROC_TARGET_SELF);
        sp->procCharges = 0;
    }

    // mage - Conjure Refreshment Table
    sp = Spell::checkAndReturnSpellEntry(43985);
    if (sp != nullptr)
        sp->EffectImplicitTargetA[0] = EFF_TARGET_DYNAMIC_OBJECT;

    // Hypothermia - forced debuff
    sp = Spell::checkAndReturnSpellEntry(41425);
    if (sp != nullptr)
    {
        sp->Attributes = ATTRIBUTES_IGNORE_INVULNERABILITY;
    }

    // Mage - Permafrost Rank 1
    sp = Spell::checkAndReturnSpellEntry(11175);
    if (sp != nullptr)
    {
        sp->EffectMiscValue[1] = SMT_MISC_EFFECT;
    }

    // Mage - Permafrost Rank 2
    sp = Spell::checkAndReturnSpellEntry(12569);
    if (sp != nullptr)
    {
        sp->EffectMiscValue[1] = SMT_MISC_EFFECT;
    }

    // Mage - Permafrost Rank 3
    sp = Spell::checkAndReturnSpellEntry(12571);
    if (sp != nullptr)
    {
        sp->EffectMiscValue[1] = SMT_MISC_EFFECT;
    }

    //Improved Counterspell rank 1
    sp = Spell::checkAndReturnSpellEntry(11255);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CAST_SPECIFIC_SPELL;
        sp->custom_ProcOnNameHash[0] = SPELL_HASH_COUNTERSPELL;
    }

    //Improved Counterspell rank 2
    sp = Spell::checkAndReturnSpellEntry(12598);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CAST_SPECIFIC_SPELL;
        sp->custom_ProcOnNameHash[0] = SPELL_HASH_COUNTERSPELL;
    }
    //////////////////////////////////////////
    // WARLOCK                                //
    //////////////////////////////////////////

    // Insert warlock spell fixes here

    //Dummy for Demonic Circle
    sp = Spell::checkAndReturnSpellEntry(48018);
    if (sp != nullptr)
    {

        sp->EffectImplicitTargetA[1] = 1;
        CreateDummySpell(62388);
        sp = Spell::checkAndReturnSpellEntry(62388);
        if (sp != nullptr)
        {
            sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
            sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY;
        }
    }

    //megai2: Immolation Aura
    sp = Spell::checkAndReturnSpellEntry(50589);
    if (sp != nullptr)
    {
        sp->ChannelInterruptFlags = 0; // Remove channeling behaviour.
    }

#if VERSION_STRING != Cata
    //megai2: Everlasting Affliction
    sp = Spell::checkAndReturnSpellEntry(47205);
    if (sp != nullptr)
    {
        sp->EffectSpellClassMask[1][0] = 0x111;
        sp->EffectSpellClassMask[1][1] = 0;
        sp->procFlags = PROC_ON_ANY_HOSTILE_ACTION;
    }

    sp = Spell::checkAndReturnSpellEntry(47204);
    if (sp != nullptr)
    {
        sp->EffectSpellClassMask[1][0] = 0x111;
        sp->EffectSpellClassMask[1][1] = 0;
        sp->procFlags = PROC_ON_ANY_HOSTILE_ACTION;
    }

    sp = Spell::checkAndReturnSpellEntry(47203);
    if (sp != nullptr)
    {
        sp->EffectSpellClassMask[1][0] = 0x111;
        sp->EffectSpellClassMask[1][1] = 0;
        sp->procFlags = PROC_ON_ANY_HOSTILE_ACTION;
    }

    sp = Spell::checkAndReturnSpellEntry(47202);
    if (sp != nullptr)
    {
        sp->EffectSpellClassMask[1][0] = 0x111;
        sp->EffectSpellClassMask[1][1] = 0;
        sp->procFlags = PROC_ON_ANY_HOSTILE_ACTION;
    }

    sp = Spell::checkAndReturnSpellEntry(47201);
    if (sp != nullptr)
    {
        sp->EffectSpellClassMask[1][0] = 0x111;
        sp->EffectSpellClassMask[1][1] = 0;
    }
#endif

    ////////////////////////////////////////////////////////////
    // Backlash
    sp = Spell::checkAndReturnSpellEntry(34936);
    if (sp != nullptr)
    {
        sp->AuraInterruptFlags = AURA_INTERRUPT_ON_CAST_SPELL;
    }

    ////////////////////////////////////////////////////////////
    // Demonic Knowledge
    sp = Spell::checkAndReturnSpellEntry(35691);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_DAMAGE_DONE;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_SELF;
        sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_DONE;
        sp->EffectBasePoints[1] = sp->EffectBasePoints[0];
        sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
        sp->Effect[2] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[2] = 35696;
        sp->EffectImplicitTargetA[2] = EFF_TARGET_PET;
    }
    sp = Spell::checkAndReturnSpellEntry(35692);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_DAMAGE_DONE;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_SELF;
        sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_DONE;
        sp->EffectBasePoints[1] = sp->EffectBasePoints[0];
        sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
        sp->Effect[2] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[2] = 35696;
        sp->EffectImplicitTargetA[2] = EFF_TARGET_PET;
    }
    sp = Spell::checkAndReturnSpellEntry(35693);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_DAMAGE_DONE;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_SELF;
        sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_DONE;
        sp->EffectBasePoints[1] = sp->EffectBasePoints[0];
        sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
        sp->Effect[2] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[2] = 35696;
        sp->EffectImplicitTargetA[2] = EFF_TARGET_PET;
    }
    sp = Spell::checkAndReturnSpellEntry(35696);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_APPLY_AURA; //making this only for the visible effect
        sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY; //no effect here
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
    }

    //Shadow Trance should be removed on the first SB
    sp = Spell::checkAndReturnSpellEntry(17941);
    if (sp != nullptr)
    {
        sp->AuraInterruptFlags = AURA_INTERRUPT_ON_CAST_SPELL;
    }

    //warlock: Empowered Corruption
    sp = Spell::checkAndReturnSpellEntry(32381);
    if (sp != nullptr)
    {
        sp->EffectBasePoints[0] *= 6;
    }
    sp = Spell::checkAndReturnSpellEntry(32382);
    if (sp != nullptr)
    {
        sp->EffectBasePoints[0] *= 6;
    }
    sp = Spell::checkAndReturnSpellEntry(32383);
    if (sp != nullptr)
    {
        sp->EffectBasePoints[0] *= 6;
    }

    //warlock - Demonic Tactics
    sp = Spell::checkAndReturnSpellEntry(30242);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_NULL; //disable this. This is just blizz crap. Pure proof that they suck :P
        sp->EffectImplicitTargetB[1] = EFF_TARGET_PET;
        //sp->EffectApplyAuraName[2] = SPELL_AURA_MOD_SPELL_CRIT_CHANCE; //lvl 1 has it fucked up :O
                                                                        // Zyres: No you fukced it up. This spell was defined few lines below.
        sp->EffectImplicitTargetB[2] = EFF_TARGET_PET;
    }
    sp = Spell::checkAndReturnSpellEntry(30245);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_NULL; //disable this. This is just blizz crap. Pure proof that they suck :P
        sp->EffectImplicitTargetB[1] = EFF_TARGET_PET;
        sp->EffectImplicitTargetB[2] = EFF_TARGET_PET;
    }
    sp = Spell::checkAndReturnSpellEntry(30246);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_NULL; //disable this. This is just blizz crap. Pure proof that they suck :P
        sp->EffectImplicitTargetB[1] = EFF_TARGET_PET;
        sp->EffectImplicitTargetB[2] = EFF_TARGET_PET;
    }
    sp = Spell::checkAndReturnSpellEntry(30247);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_NULL; //disable this. This is just blizz crap. Pure proof that they suck :P
        sp->EffectImplicitTargetB[1] = EFF_TARGET_PET;
        sp->EffectImplicitTargetB[2] = EFF_TARGET_PET;
    }
    sp = Spell::checkAndReturnSpellEntry(30248);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_NULL; //disable this. This is just blizz crap. Pure proof that they suck :P
        sp->EffectImplicitTargetB[1] = EFF_TARGET_PET;
        sp->EffectImplicitTargetB[2] = EFF_TARGET_PET;
    }

    //warlock - Demonic Resilience
    sp = Spell::checkAndReturnSpellEntry(30319);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
    }
    sp = Spell::checkAndReturnSpellEntry(30320);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
    }
    sp = Spell::checkAndReturnSpellEntry(30321);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
    }

    //warlock - Improved Imp
    sp = Spell::checkAndReturnSpellEntry(18694);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
    }
    sp = Spell::checkAndReturnSpellEntry(18695);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
    }
    sp = Spell::checkAndReturnSpellEntry(18696);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
    }

    //warlock - Demonic Brutality
    sp = Spell::checkAndReturnSpellEntry(18705);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
    }
    sp = Spell::checkAndReturnSpellEntry(18706);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
    }
    sp = Spell::checkAndReturnSpellEntry(18707);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
    }

    //warlock - Improved Succubus
    sp = Spell::checkAndReturnSpellEntry(18754);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
    }
    sp = Spell::checkAndReturnSpellEntry(18755);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
    }
    sp = Spell::checkAndReturnSpellEntry(18756);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
    }

    //warlock - Fel Vitality
    sp = Spell::checkAndReturnSpellEntry(18731);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_PERCENT_STAT;
        sp->EffectMiscValue[0] = 3;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
    }
    sp = Spell::checkAndReturnSpellEntry(18743);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_PERCENT_STAT;
        sp->EffectMiscValue[0] = 3;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
    }
    sp = Spell::checkAndReturnSpellEntry(18744);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_PERCENT_STAT;
        sp->EffectMiscValue[0] = 3;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
    }

    //warlock - Demonic Tactics
    /* Zyres: Disabled this spell has already some changes few lines above!
    sp = checkAndReturnSpellEntry(30242);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
        //this is required since blizz uses spells for melee attacks while we use fixed functions
        sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
        sp->EffectMiscValue[1] = SCHOOL_NORMAL;
        sp->EffectBasePoints[1] = sp->EffectBasePoints[0];
    }*/

    //warlock - Unholy Power
    sp = Spell::checkAndReturnSpellEntry(18769);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
        //this is required since blizz uses spells for melee attacks while we use fixed functions
        sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
        sp->EffectMiscValue[1] = SCHOOL_NORMAL;
        sp->EffectBasePoints[1] = sp->EffectBasePoints[0];
    }
    sp = Spell::checkAndReturnSpellEntry(18770);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
        //this is required since blizz uses spells for melee attacks while we use fixed functions
        sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
        sp->EffectMiscValue[1] = SCHOOL_NORMAL;
        sp->EffectBasePoints[1] = sp->EffectBasePoints[0];
    }
    sp = Spell::checkAndReturnSpellEntry(18771);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
        //this is required since blizz uses spells for melee attacks while we use fixed functions
        sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
        sp->EffectMiscValue[1] = SCHOOL_NORMAL;
        sp->EffectBasePoints[1] = sp->EffectBasePoints[0];
    }
    sp = Spell::checkAndReturnSpellEntry(18772);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
        //this is required since blizz uses spells for melee attacks while we use fixed functions
        sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
        sp->EffectMiscValue[1] = SCHOOL_NORMAL;
        sp->EffectBasePoints[1] = sp->EffectBasePoints[0];
    }
    sp = Spell::checkAndReturnSpellEntry(18773);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
        //this is required since blizz uses spells for melee attacks while we use fixed functions
        sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
        sp->EffectMiscValue[1] = SCHOOL_NORMAL;
        sp->EffectBasePoints[1] = sp->EffectBasePoints[0];
    }

    //warlock - Master Demonologist - 25 spells here
    sp = Spell::checkAndReturnSpellEntry(23785);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 23784;
    }
    sp = Spell::checkAndReturnSpellEntry(23822);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 23830;
    }
    sp = Spell::checkAndReturnSpellEntry(23823);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 23831;
    }
    sp = Spell::checkAndReturnSpellEntry(23824);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 23832;
    }
    sp = Spell::checkAndReturnSpellEntry(23825);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 35708;
    }
    //and the rest
    sp = Spell::checkAndReturnSpellEntry(23784);
    if (sp != nullptr)
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
    sp = Spell::checkAndReturnSpellEntry(23830);
    if (sp != nullptr)
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
    sp = Spell::checkAndReturnSpellEntry(23831);
    if (sp != nullptr)
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
    sp = Spell::checkAndReturnSpellEntry(23832);
    if (sp != nullptr)
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
    sp = Spell::checkAndReturnSpellEntry(35708);
    if (sp != nullptr)
        sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
    sp = Spell::checkAndReturnSpellEntry(23759);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
    }
    sp = Spell::checkAndReturnSpellEntry(23760);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
    }
    sp = Spell::checkAndReturnSpellEntry(23761);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
    }
    sp = Spell::checkAndReturnSpellEntry(23762);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
    }
    sp = Spell::checkAndReturnSpellEntry(23826);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
    }
    sp = Spell::checkAndReturnSpellEntry(23827);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
    }
    sp = Spell::checkAndReturnSpellEntry(23828);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
    }
    sp = Spell::checkAndReturnSpellEntry(23829);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
    }
    // Zyres: eeek
    for (uint32 i = 23833; i <= 23844; ++i)
    {
        sp = Spell::checkAndReturnSpellEntry(i);
        if (sp != nullptr)
        {
            sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
        }
    }
    sp = Spell::checkAndReturnSpellEntry(35702);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
        sp->Effect[1] = SPELL_EFFECT_NULL; //hacks, we are handling this in another way
    }
    sp = Spell::checkAndReturnSpellEntry(35703);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
        sp->Effect[1] = SPELL_EFFECT_NULL; //hacks, we are handling this in another way
    }
    sp = Spell::checkAndReturnSpellEntry(35704);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
        sp->Effect[1] = SPELL_EFFECT_NULL; //hacks, we are handling this in another way
    }
    sp = Spell::checkAndReturnSpellEntry(35705);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
        sp->Effect[1] = SPELL_EFFECT_NULL; //hacks, we are handling this in another way
    }
    sp = Spell::checkAndReturnSpellEntry(35706);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
        sp->Effect[1] = SPELL_EFFECT_NULL; //hacks, we are handling this in another way
    }

    //warlock - Improved Drain Soul
    sp = Spell::checkAndReturnSpellEntry(18213);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_TARGET_DIE | static_cast<uint32>(PROC_TARGET_SELF);
        sp->procChance = 100;
        sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 18371;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_SELF;
        sp->Effect[2] = SPELL_EFFECT_NULL; //remove this effect
    }
    sp = Spell::checkAndReturnSpellEntry(18372);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_TARGET_DIE | static_cast<uint32>(PROC_TARGET_SELF);
        sp->procChance = 100;
        sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 18371;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_SELF;
        sp->Effect[2] = SPELL_EFFECT_NULL; //remove this effect
    }

    //Warlock Chaos bolt
    sp = Spell::checkAndReturnSpellEntry(50796);
    if (sp != nullptr)
    {
        sp->Attributes |= ATTRIBUTES_IGNORE_INVULNERABILITY;
        sp->School = SCHOOL_FIRE;
    }

    sp = Spell::checkAndReturnSpellEntry(59170);
    if (sp != nullptr)
    {
        sp->Attributes |= ATTRIBUTES_IGNORE_INVULNERABILITY;
        sp->School = SCHOOL_FIRE;
    }

    sp = Spell::checkAndReturnSpellEntry(59171);
    if (sp != nullptr)
    {
        sp->Attributes |= ATTRIBUTES_IGNORE_INVULNERABILITY;
        sp->School = SCHOOL_FIRE;
    }

    sp = Spell::checkAndReturnSpellEntry(59172);
    if (sp != nullptr)
    {
        sp->Attributes |= ATTRIBUTES_IGNORE_INVULNERABILITY;
        sp->School = SCHOOL_FIRE;
    }
    // End Warlock chaos bolt

    //Warlock Healthstones
    int HealthStoneID[8] = { 6201, 6202, 5699, 11729, 11730, 27230, 47871, 47878 };
    for (uint8 i = 0; i < 8; i++)
    {
        sp = Spell::checkAndReturnSpellEntry(HealthStoneID[i]);
        if (sp != nullptr)
        {
            sp->Reagent[1] = 0;
        }
    }

    //////////////////////////////////////////
    // DRUID                                //
    //////////////////////////////////////////

    // Insert druid spell fixes here

    ////////////////////////////////////////////////////////////
    // Balance
    ////////////////////////////////////////////////////////////

    // Druid - Force of Nature
    sp = Spell::checkAndReturnSpellEntry(33831);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_SELF; //some land under target is used that gathers multiple targets ...
        sp->EffectImplicitTargetA[1] = EFF_TARGET_NONE;
    }

    ////////////////////////////////////////////////////////////
    //    Feral Combat
    ////////////////////////////////////////////////////////////

    // Druid - Infected Wounds
    sp = Spell::checkAndReturnSpellEntry(48483);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CAST_SPECIFIC_SPELL;
        sp->custom_ProcOnNameHash[0] = SPELL_HASH_SHRED;
        sp->custom_ProcOnNameHash[1] = SPELL_HASH_MAUL;
        sp->custom_ProcOnNameHash[2] = SPELL_HASH_MANGLE;
    }

    sp = Spell::checkAndReturnSpellEntry(48484);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CAST_SPECIFIC_SPELL;
        sp->custom_ProcOnNameHash[0] = SPELL_HASH_SHRED;
        sp->custom_ProcOnNameHash[1] = SPELL_HASH_MAUL;
        sp->custom_ProcOnNameHash[2] = SPELL_HASH_MANGLE;
    }

    sp = Spell::checkAndReturnSpellEntry(48485);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CAST_SPECIFIC_SPELL;
        sp->custom_ProcOnNameHash[0] = SPELL_HASH_SHRED;
        sp->custom_ProcOnNameHash[1] = SPELL_HASH_MAUL;
        sp->custom_ProcOnNameHash[2] = SPELL_HASH_MANGLE;
    }

    // Druid - Bash - Interrupt effect
    sp = Spell::checkAndReturnSpellEntry(5211);
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 32747;
    }
    sp = Spell::checkAndReturnSpellEntry(6798);
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 32747;
    }
    sp = Spell::checkAndReturnSpellEntry(8983);
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 32747;
    }

    //Druid - Feral Swiftness
    sp = Spell::checkAndReturnSpellEntry(17002);
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 24867;
    }
    sp = Spell::checkAndReturnSpellEntry(24866);
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 24864;
    }

    // Druid - Maim
    sp = Spell::checkAndReturnSpellEntry(22570);
    if (sp != nullptr)
    {
        sp->AuraInterruptFlags = AURA_INTERRUPT_ON_UNUSED2;
        sp->custom_is_melee_spell = true;
    }
    sp = Spell::checkAndReturnSpellEntry(49802);
    if (sp != nullptr)
    {
        sp->AuraInterruptFlags = AURA_INTERRUPT_ON_UNUSED2;
        sp->custom_is_melee_spell = true;
    }

    sp = Spell::checkAndReturnSpellEntry(20719); //feline grace
    if (sp != nullptr)
        sp->Effect[0] = SPELL_EFFECT_NULL;

    // Druid - Feral Swiftness
    sp = Spell::checkAndReturnSpellEntry(17002);
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 24867;
    }
    sp = Spell::checkAndReturnSpellEntry(24866);
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 24864;
    }

    // Druid - Frenzied Regeneration
    sp = Spell::checkAndReturnSpellEntry(22842);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[0] = SPELL_AURA_PERIODIC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 22845;
    }

    // Druid - Primal Fury (talent)
    sp = Spell::checkAndReturnSpellEntry(37116);
    if (sp != nullptr)
        sp->RequiredShapeShift = 0;

    sp = Spell::checkAndReturnSpellEntry(37117);
    if (sp != nullptr)
        sp->RequiredShapeShift = 0;

    // Druid - Predatory Strikes
    uint32 mm = decimalToMask(FORM_BEAR) | decimalToMask(FORM_DIREBEAR) | decimalToMask(FORM_MOONKIN) | decimalToMask(FORM_CAT);

    sp = Spell::checkAndReturnSpellEntry(16972);
    if (sp != nullptr)
        sp->RequiredShapeShift = mm;
    sp = Spell::checkAndReturnSpellEntry(16974);
    if (sp != nullptr)
        sp->RequiredShapeShift = mm;
    sp = Spell::checkAndReturnSpellEntry(16975);
    if (sp != nullptr)
        sp->RequiredShapeShift = mm;

    ////////////////////////////////////////////////////////////
    // Restoration
    ////////////////////////////////////////////////////////////

    // Druid - Tree Form Aura
    /* Zyres: Genius... Delete this! I'm not familiar with this technique, looks awesome. Unfortunately I don't understand the effect of this. SPELL_HASH_TREE_OF_LIFE is not used in any statement...
    sp = checkAndReturnSpellEntry(34123);
    if (sp != nullptr)
        sp->custom_NameHash = 0;*/

    // Druid - Natural Shapeshifter
    sp = Spell::checkAndReturnSpellEntry(16833);
    if (sp != nullptr)
        sp->DurationIndex = 0;
    sp = Spell::checkAndReturnSpellEntry(16834);
    if (sp != nullptr)
        sp->DurationIndex = 0;
    sp = Spell::checkAndReturnSpellEntry(16835);
    if (sp != nullptr)
        sp->DurationIndex = 0;

    sp = Spell::checkAndReturnSpellEntry(61177); // Northrend Inscription Research
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_NULL;
        //sp->EffectBaseDice[1] = 0;
        sp->EffectBasePoints[1] = 0;
        sp->EffectImplicitTargetA[1] = 0;
        sp->EffectDieSides[1] = 0;
    }
    sp = Spell::checkAndReturnSpellEntry(61288); // Minor Inscription Research
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_NULL;
        //sp->EffectBaseDice[1] = 0;
        sp->EffectBasePoints[1] = 0;
        sp->EffectImplicitTargetA[1] = 0;
        sp->EffectDieSides[1] = 0;
    }
    sp = Spell::checkAndReturnSpellEntry(60893); // Northrend Alchemy Research
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_NULL;
        //sp->EffectBaseDice[1] = 0;
        sp->EffectBasePoints[1] = 0;
        sp->EffectImplicitTargetA[1] = 0;
        sp->EffectDieSides[1] = 0;
    }
#if VERSION_STRING != Cata
    sp = Spell::checkAndReturnSpellEntry(46097); // Brutal Totem of Survival
    if (sp != nullptr)
    {
        sp->EffectSpellClassMask[0][0] = 0x00100000 | 0x10000000 | 0x80000000;
        sp->EffectSpellClassMask[0][1] = 0x08000000;
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_SELF;
    }
    sp = Spell::checkAndReturnSpellEntry(43860); // Totem of Survival
    if (sp != nullptr)
    {
        sp->EffectSpellClassMask[0][0] = 0x00100000 | 0x10000000 | 0x80000000;
        sp->EffectSpellClassMask[0][1] = 0x08000000;
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_SELF;
    }
    sp = Spell::checkAndReturnSpellEntry(43861); // Merciless Totem of Survival
    if (sp != nullptr)
    {
        sp->EffectSpellClassMask[0][0] = 0x00100000 | 0x10000000 | 0x80000000;
        sp->EffectSpellClassMask[0][1] = 0x08000000;
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_SELF;
    }
    sp = Spell::checkAndReturnSpellEntry(43862); // Vengeful Totem of Survival
    if (sp != nullptr)
    {
        sp->EffectSpellClassMask[0][0] = 0x00100000 | 0x10000000 | 0x80000000;
        sp->EffectSpellClassMask[0][1] = 0x08000000;
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_SELF;
    }
    sp = Spell::checkAndReturnSpellEntry(60564); // Savage Gladiator's Totem of Survival
    if (sp != nullptr)
    {
        sp->EffectSpellClassMask[0][0] = 0x00100000 | 0x10000000 | 0x80000000;
        sp->EffectSpellClassMask[0][1] = 0x08000000;
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_SELF;
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 60565; // Savage Magic
    }
    sp = Spell::checkAndReturnSpellEntry(60571); // Hateful Gladiator's Totem of Survival
    if (sp != nullptr)
    {
        sp->EffectSpellClassMask[0][0] = 0x00100000 | 0x10000000 | 0x80000000;
        sp->EffectSpellClassMask[0][1] = 0x08000000;
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_SELF;
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 60566; // Hateful Magic
    }
    sp = Spell::checkAndReturnSpellEntry(60572); // Deadly Gladiator's Totem of Survival
    if (sp != nullptr)
    {
        sp->EffectSpellClassMask[0][0] = 0x00100000 | 0x10000000 | 0x80000000;
        sp->EffectSpellClassMask[0][1] = 0x08000000;
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_SELF;
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 60567; // Deadly Magic
    }
    sp = Spell::checkAndReturnSpellEntry(60567); // Deadly Magic
    if (sp != nullptr)
        sp->EffectImplicitTargetA[1] = EFF_TARGET_SELF;
    sp = Spell::checkAndReturnSpellEntry(46098); // Brutal Totem of Third WInd
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectSpellClassMask[0][0] = 0x00000080;
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 46099; // Brutal Gladiator's Totem of the Third Wind
    }
    sp = Spell::checkAndReturnSpellEntry(34138); // Totem of the Third Wind
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectSpellClassMask[0][0] = 0x00000080;
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 34132; // Gladiator's Totem of the Third Wind
    }
    sp = Spell::checkAndReturnSpellEntry(42370); // Merciless Totem of the Third WInd
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectSpellClassMask[0][0] = 0x00000080;
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 42371; // Merciless Gladiator's Totem of the Third Wind
    }
    sp = Spell::checkAndReturnSpellEntry(43728); // Vengeful Totem of Third WInd
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectSpellClassMask[0][0] = 0x00000080;
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 43729; // Vengeful Gladiator's Totem of the Third Wind
    }
#endif
    //////////////////////////////////////////
    // ITEMS                                //
    //////////////////////////////////////////

    // Insert items spell fixes here

    //Compact Harvest Reaper
    sp = Spell::checkAndReturnSpellEntry(4078);
    if (sp != nullptr)
    {
        sp->DurationIndex = 6;
    }

    //Graccu's Mince Meat Fruitcake
    sp = Spell::checkAndReturnSpellEntry(25990);
    if (sp != nullptr)
    {
        sp->EffectAmplitude[1] = 1000;
    }

    //Extract Gas
    sp = Spell::checkAndReturnSpellEntry(30427);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_DUMMY;
    }

    //Relic - Idol of the Unseen Moon
    sp = Spell::checkAndReturnSpellEntry(43739);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CAST_SPELL;
    }

    //Lunar Grace - Idol of the Unseen Moon proc
    sp = Spell::checkAndReturnSpellEntry(43740);
    if (sp != nullptr)
    {
        sp->custom_ProcOnNameHash[0] = SPELL_HASH_MOONFIRE;
    }

    //Primal Instinct - Idol of Terror proc
    sp = Spell::checkAndReturnSpellEntry(43738);
    if (sp != nullptr)
    {
        sp->custom_self_cast_only = true;
        sp->custom_ProcOnNameHash[0] = SPELL_HASH_MANGLE__CAT_;
        sp->custom_ProcOnNameHash[1] = SPELL_HASH_MANGLE__BEAR_;
    }

    //Thunderfury
    sp = Spell::checkAndReturnSpellEntry(21992);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[2] = EFF_TARGET_ALL_ENEMIES_AROUND_CASTER; // cebernic: for enemies not self
    }

    // Sigil of the Unfaltering Knight
    sp = Spell::checkAndReturnSpellEntry(62147);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CAST_SPECIFIC_SPELL;
        sp->custom_ProcOnNameHash[0] = SPELL_HASH_ICY_TOUCH;
        sp->custom_proc_interval = 45000;
    }

    // Deadly Aggression - triggered by Deadly Gladiator's Relic/Idol/Libram/Totem
    sp = Spell::checkAndReturnSpellEntry(60549);
    if (sp != nullptr)
    {
        // effect 1 and 2 are the same... dunno why
        sp->Effect[1] = SPELL_EFFECT_NULL;
    }

    // Furious Gladiator's Libram of Fortitude - triggered by LK Arena 4 Gladiator's Relic/Idol/Libram/Totem
    sp = Spell::checkAndReturnSpellEntry(60551);
    if (sp != nullptr)
    {
        // effect 1 and 2 are the same... dunno why
        sp->Effect[1] = SPELL_EFFECT_NULL;
    }

    // Relentless Aggression - triggered by LK Arena 5 Gladiator's Relic/Idol/Libram/Totem
    sp = Spell::checkAndReturnSpellEntry(60553);
    if (sp != nullptr)
    {
        // effect 1 and 2 are the same... dunno why
        sp->Effect[1] = SPELL_EFFECT_NULL;
    }

    // Savage Aggression - triggered by Savage Gladiator's Relic/Idol/Libram/Totem
    sp = Spell::checkAndReturnSpellEntry(60544);
    if (sp != nullptr)
    {
        // effect 1 and 2 are the same... dunno why
        sp->Effect[1] = SPELL_EFFECT_NULL;
    }

    // Sigil of Haunted Dreams
    sp = Spell::checkAndReturnSpellEntry(60826);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CAST_SPECIFIC_SPELL;
        sp->custom_ProcOnNameHash[0] = SPELL_HASH_BLOOD_STRIKE;
        sp->custom_ProcOnNameHash[1] = SPELL_HASH_HEART_STRIKE;
        sp->custom_proc_interval = 45000;
    }

    //Totem of the Third Wind - bad range
    sp = Spell::checkAndReturnSpellEntry(34132);
    if (sp != nullptr)
    {
        sp->rangeIndex = 5;
    }
    sp = Spell::checkAndReturnSpellEntry(42371);
    if (sp != nullptr)
    {
        sp->rangeIndex = 5;
    }
    sp = Spell::checkAndReturnSpellEntry(43729);
    if (sp != nullptr)
    {
        sp->rangeIndex = 5;
    }
    sp = Spell::checkAndReturnSpellEntry(46099);
    if (sp != nullptr)
    {
        sp->rangeIndex = 5;
    }

    // Eye of Acherus, our phase shift mode messes up the control :/
    sp = Spell::checkAndReturnSpellEntry(51852);
    if (sp != nullptr)
        sp->Effect[0] = SPELL_EFFECT_NULL;


    //Ashtongue Talisman of Equilibrium
    // DankoDJ: To set the same value several times makes no sense!
    sp = Spell::checkAndReturnSpellEntry(40442);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->procChance = 40;
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectTriggerSpell[0] = 40452;
        sp->maxstack = 1;
        sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->procChance = 25;
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectTriggerSpell[1] = 40445;
        sp->maxstack = 1;
        sp->Effect[2] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[2] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->procChance = 25;
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectTriggerSpell[2] = 40446;
        sp->maxstack = 1;
    }

    //Ashtongue Talisman of Acumen
    // DankoDJ: To set the same value several times makes no sense!
    sp = Spell::checkAndReturnSpellEntry(40438);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->procChance = 10;
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectTriggerSpell[0] = 40441;
        sp->maxstack = 1;
        sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->procChance = 10;
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectTriggerSpell[1] = 40440;
        sp->maxstack = 1;
    }
    // Drums of war targets surrounding party members instead of us
    sp = Spell::checkAndReturnSpellEntry(35475);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_ALL_PARTY;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_ALL_PARTY;
        sp->EffectImplicitTargetA[2] = 0;
        sp->EffectImplicitTargetB[0] = 0;
        sp->EffectImplicitTargetB[1] = 0;
        sp->EffectImplicitTargetB[2] = 0;
    }

    // Drums of Battle targets surrounding party members instead of us
    sp = Spell::checkAndReturnSpellEntry(35476);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_ALL_PARTY;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_ALL_PARTY;
        sp->EffectImplicitTargetA[2] = 0;
        sp->EffectImplicitTargetB[0] = 0;
        sp->EffectImplicitTargetB[1] = 0;
        sp->EffectImplicitTargetB[2] = 0;
    }

    // Drums of Panic targets surrounding creatures instead of us
    sp = Spell::checkAndReturnSpellEntry(35474);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_ALL_ENEMIES_AROUND_CASTER;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_ALL_ENEMIES_AROUND_CASTER;
        sp->EffectImplicitTargetA[2] = 0;
        sp->EffectImplicitTargetB[0] = 0;
        sp->EffectImplicitTargetB[1] = 0;
        sp->EffectImplicitTargetB[2] = 0;
    }

    // Drums of Restoration targets surrounding party members instead of us
    sp = Spell::checkAndReturnSpellEntry(35478);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_ALL_PARTY;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_ALL_PARTY;
        sp->EffectImplicitTargetA[2] = 0;
        sp->EffectImplicitTargetB[0] = 0;
        sp->EffectImplicitTargetB[1] = 0;
        sp->EffectImplicitTargetB[2] = 0;
    }
    // Drums of Speed targets surrounding party members instead of us
    sp = Spell::checkAndReturnSpellEntry(35477);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_ALL_PARTY;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_ALL_PARTY;
        sp->EffectImplicitTargetA[2] = 0;
        sp->EffectImplicitTargetB[0] = 0;
        sp->EffectImplicitTargetB[1] = 0;
        sp->EffectImplicitTargetB[2] = 0;
    }

    //all Drums
    sp = Spell::checkAndReturnSpellEntry(35474);
    if (sp != nullptr)
        sp->RequiredShapeShift = 0;
    sp = Spell::checkAndReturnSpellEntry(35475);
    if (sp != nullptr)
        sp->RequiredShapeShift = 0;
    sp = Spell::checkAndReturnSpellEntry(35476);
    if (sp != nullptr)
        sp->RequiredShapeShift = 0;
    sp = Spell::checkAndReturnSpellEntry(35477);
    if (sp != nullptr)
        sp->RequiredShapeShift = 0;
    sp = Spell::checkAndReturnSpellEntry(35478);
    if (sp != nullptr)
        sp->RequiredShapeShift = 0;

    //Purify helboar meat
    sp = Spell::checkAndReturnSpellEntry(29200);
    if (sp != nullptr)
    {
        sp->Reagent[1] = 0;
        sp->ReagentCount[1] = 0;
    }

    //Thorium Grenade
    sp = Spell::checkAndReturnSpellEntry(19769);
    if (sp != nullptr)
    {
        sp->InterruptFlags |= ~(CAST_INTERRUPT_ON_MOVEMENT);
    }

    //M73 Frag Grenade
    sp = Spell::checkAndReturnSpellEntry(13808);
    if (sp != nullptr)
    {
        sp->InterruptFlags |= ~(CAST_INTERRUPT_ON_MOVEMENT);
    }

    //Iron Grenade
    sp = Spell::checkAndReturnSpellEntry(4068);
    if (sp != nullptr)
    {
        sp->InterruptFlags |= ~(CAST_INTERRUPT_ON_MOVEMENT);
    }

    //Frost Grenade
    sp = Spell::checkAndReturnSpellEntry(39965);
    if (sp != nullptr)
    {
        sp->InterruptFlags |= ~(CAST_INTERRUPT_ON_MOVEMENT);
    }

    //Adamantine Grenade
    sp = Spell::checkAndReturnSpellEntry(30217);
    if (sp != nullptr)
    {
        sp->InterruptFlags |= ~(CAST_INTERRUPT_ON_MOVEMENT);
    }

    ///////////////////////////////////////////////////////////////
    // Trinket Fixes        Please keep nice and clean :)
    ///////////////////////////////////////////////////////////////

    // Citrine Pendant of Golden Healing
    sp = Spell::checkAndReturnSpellEntry(25608);        //    http://www.wowhead.com/?item=20976
    if (sp != nullptr)
    {
        //Overrides any spell coefficient calculation - DBCStores.h
        sp->Dspell_coef_override = 0;    //DD&DH
        sp->OTspell_coef_override = 0;    //HOT&DOT
    }

    //Figurine - Shadowsong Panther
    sp = Spell::checkAndReturnSpellEntry(46784);        //    http://www.wowhead.com/?item=35702
    if (sp != nullptr)
        sp->AttributesEx |= ATTRIBUTESEX_NOT_BREAK_STEALTH;

    // Infernal Protection
    sp = Spell::checkAndReturnSpellEntry(36488);            //    http://www.wowhead.com/?spell=36488
    if (sp != nullptr)
        sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_FRIEND;


    //Fury of the Five Flights
    sp = Spell::checkAndReturnSpellEntry(60313);
    if (sp != nullptr)
    {
        sp->maxstack = 20;
    }

    //Pendant of the Violet Eye
    sp = Spell::checkAndReturnSpellEntry(35095);
    if (sp != nullptr)
    {
        sp->custom_self_cast_only = true;
    }

    //////////////////////////////////////////
    // BOSSES                                //
    //////////////////////////////////////////

    // Insert boss spell fixes here

    // Major Domo - Magic Reflection
    sp = Spell::checkAndReturnSpellEntry(20619);
    if (sp != nullptr)
    {
        for (uint8 i = 0; i < 3; ++i)
        {
            if (sp->EffectImplicitTargetA[i] > 0)
                sp->EffectImplicitTargetA[i] = EFF_TARGET_ALL_FRIENDLY_IN_AREA;
            if (sp->EffectImplicitTargetB[i] > 0)
                sp->EffectImplicitTargetB[i] = EFF_TARGET_ALL_FRIENDLY_IN_AREA;
        }
    }

    // Major Domo - Damage Shield
    sp = Spell::checkAndReturnSpellEntry(21075);
    if (sp != nullptr)
    {
        for (uint8 i = 0; i < 3; ++i)
        {
            if (sp->EffectImplicitTargetA[i] > 0)
                sp->EffectImplicitTargetA[i] = EFF_TARGET_ALL_FRIENDLY_IN_AREA;
            if (sp->EffectImplicitTargetB[i] > 0)
                sp->EffectImplicitTargetB[i] = EFF_TARGET_ALL_FRIENDLY_IN_AREA;
        }
    }

    // Dark Glare
    sp = Spell::checkAndReturnSpellEntry(26029);
    if (sp != nullptr)
        sp->cone_width = 15.0f; // 15 degree cone

    // Drain Power (Malacrass) // bugged - the charges fade even when refreshed with new ones. This makes them everlasting.
    sp = Spell::checkAndReturnSpellEntry(44131);
    if (sp != nullptr)
        sp->DurationIndex = 21;
    sp = Spell::checkAndReturnSpellEntry(44132);
    if (sp != nullptr)
        sp->DurationIndex = 21;

    // Zul'jin spell, proc from Creeping Paralysis
    sp = Spell::checkAndReturnSpellEntry(43437);
    if (sp != nullptr)
    {
        sp->EffectImplicitTargetA[0] = 0;
        sp->EffectImplicitTargetA[1] = 0;
    }

    //Bloodboil
    sp = Spell::checkAndReturnSpellEntry(42005);
    if (sp != nullptr)
    {
        sp->MaxTargets = 5;
    }

    //Doom
    sp = Spell::checkAndReturnSpellEntry(31347);
    if (sp != nullptr)
    {
        sp->MaxTargets = 1;
    }
    //Shadow of Death
    sp = Spell::checkAndReturnSpellEntry(40251);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_PERIODIC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 0;
    }

    sp = Spell::checkAndReturnSpellEntry(9036);
    if (sp != nullptr)
    {
        sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 20584;
    }

    sp = Spell::checkAndReturnSpellEntry(24379);   //bg Restoration
    if (sp != nullptr)
    {
        sp->EffectTriggerSpell[0] = 23493;
    }

    sp = Spell::checkAndReturnSpellEntry(23493);   //bg Restoration
    if (sp != nullptr)
    {
        sp->EffectTriggerSpell[0] = 24379;
    }

    sp = Spell::checkAndReturnSpellEntry(5246);    // why self?
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 20511; // cebernic: this just real spell
        sp->EffectImplicitTargetA[0] = EFF_TARGET_NONE;
    }

    //////////////////////////////////////////
    // DEATH KNIGHT                            //
    //////////////////////////////////////////

    // Insert Death Knight spells here

    // Unholy Aura - Ranks 1
    sp = Spell::checkAndReturnSpellEntry(50391);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_INCREASE_SPEED_ALWAYS;
        sp->Effect[0] = SPELL_EFFECT_APPLY_GROUP_AREA_AURA;
        sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 50392;
        sp->Effect[1] = SPELL_EFFECT_APPLY_GROUP_AREA_AURA;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_SELF;
    }
    // Unholy Aura - Ranks 2
    sp = Spell::checkAndReturnSpellEntry(50392);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_INCREASE_SPEED_ALWAYS;
        sp->Effect[0] = SPELL_EFFECT_APPLY_GROUP_AREA_AURA;
        sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[1] = 50392;
        sp->Effect[1] = SPELL_EFFECT_APPLY_GROUP_AREA_AURA;
        sp->EffectImplicitTargetA[1] = EFF_TARGET_SELF;
    }

    //    Empower Rune Weapon
    sp = Spell::checkAndReturnSpellEntry(47568);
    if (sp != nullptr)
    {
        sp->Effect[2] = SPELL_EFFECT_ACTIVATE_RUNES;
        sp->EffectBasePoints[2] = 1;
        sp->EffectMiscValue[2] = RUNE_UNHOLY;
    }

    // Frost Presence
    sp = Spell::checkAndReturnSpellEntry(48263);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_BASE_RESISTANCE_PCT;
        sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT;
        sp->EffectBasePoints[1] = 9;
        sp->EffectApplyAuraName[2] = SPELL_AURA_MOD_DAMAGE_TAKEN;
        sp->custom_BGR_one_buff_from_caster_on_self = SPELL_TYPE3_DEATH_KNIGHT_AURA;
    }

    //    Unholy Presence
    sp = Spell::checkAndReturnSpellEntry(48265);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_HASTE;
        sp->EffectBasePoints[0] = 14;
        sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_INCREASE_SPEED;
        sp->EffectBasePoints[1] = 14;
        sp->custom_BGR_one_buff_from_caster_on_self = SPELL_TYPE3_DEATH_KNIGHT_AURA;
    }

    // DEATH AND DECAY
    sp = sSpellCustomizations.GetSpellInfo(49937);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_PERIODIC_DAMAGE;
        sp->Effect[0] = SPELL_EFFECT_PERSISTENT_AREA_AURA;
    }

    sp = sSpellCustomizations.GetSpellInfo(49936);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_PERIODIC_DAMAGE;
        sp->Effect[0] = SPELL_EFFECT_PERSISTENT_AREA_AURA;
    }

    sp = sSpellCustomizations.GetSpellInfo(49938);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_PERIODIC_DAMAGE;
        sp->Effect[0] = SPELL_EFFECT_PERSISTENT_AREA_AURA;
    }

    sp = sSpellCustomizations.GetSpellInfo(43265);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_PERIODIC_DAMAGE;
        sp->Effect[0] = SPELL_EFFECT_PERSISTENT_AREA_AURA;
    }

    // Runic Empowerment
    /*sp = sSpellCustomizations.GetSpellInfo(81229);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 81229;
        sp->custom_ProcOnNameHash[0] = SPELL_HASH_DEATH_STRIKE;
        sp->custom_ProcOnNameHash[1] = SPELL_HASH_FROST_STRIKE;
        sp->custom_ProcOnNameHash[2] = SPELL_HASH_DEATH_COIL;
        sp->procChance = 45;
    }*/

    // Vengeance
    sp = sSpellCustomizations.GetSpellInfo(93099);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 76691;
    }

    ///////////////////////////////////////////////////////////
    //    Path of Frost
    ///////////////////////////////////////////////////////////
    sp = Spell::checkAndReturnSpellEntry(3714);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_WATER_WALK;
        sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
    }

    // Rune Strike
    sp = Spell::checkAndReturnSpellEntry(56815);
    if (sp != nullptr)
    {
        sp->Attributes |= ATTRIBUTES_CANT_BE_DPB;
    }

    CreateDummySpell(56817);
    sp = Spell::checkAndReturnSpellEntry(56817);
    if (sp != nullptr)
    {
        sp->DurationIndex = 28;
        sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
        sp->EffectApplyAuraName[0] = SPELL_AURA_DUMMY;
    }

    //Frost Strike
    sp = Spell::checkAndReturnSpellEntry(49143);
    if (sp != nullptr)
    {
        sp->Attributes = ATTRIBUTES_CANT_BE_DPB;
    }
    sp = Spell::checkAndReturnSpellEntry(51416);
    if (sp != nullptr)
    {
        sp->Attributes = ATTRIBUTES_CANT_BE_DPB;
    }
    sp = Spell::checkAndReturnSpellEntry(51417);
    if (sp != nullptr)
    {
        sp->Attributes = ATTRIBUTES_CANT_BE_DPB;
    }
    sp = Spell::checkAndReturnSpellEntry(51418);
    if (sp != nullptr)
    {
        sp->Attributes = ATTRIBUTES_CANT_BE_DPB;
    }
    sp = Spell::checkAndReturnSpellEntry(51419);
    if (sp != nullptr)
    {
        sp->Attributes = ATTRIBUTES_CANT_BE_DPB;
    }
    sp = Spell::checkAndReturnSpellEntry(55268);
    if (sp != nullptr)
    {
        sp->Attributes = ATTRIBUTES_CANT_BE_DPB;
    }

    // Noggenfogger elixir - reduce size effect
    sp = Spell::checkAndReturnSpellEntry(16595);
    if (sp != nullptr)
    {
        sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_SCALE;
        sp->EffectBasePoints[0] = -50;
        sp->maxstack = 1;
    }

    sp = Spell::checkAndReturnSpellEntry(46584);
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_DUMMY;
        sp->Effect[1] = SPELL_EFFECT_NULL;
        sp->Effect[2] = SPELL_EFFECT_NULL;
    }

    //Other Librams
    //Libram of Saints Departed and Libram of Zeal
    sp = Spell::checkAndReturnSpellEntry(34263);
    if (sp != nullptr)
    {
        sp->custom_self_cast_only = true;
        sp->custom_ProcOnNameHash[0] = SPELL_HASH_JUDGEMENT;
        sp->procChance = 100;
    }

    //Libram of Avengement
    sp = Spell::checkAndReturnSpellEntry(34260);
    if (sp != nullptr)
    {
        sp->custom_self_cast_only = true;
        sp->custom_ProcOnNameHash[0] = SPELL_HASH_JUDGEMENT;
        sp->procChance = 100;
    }

    //Libram of Mending
    sp = Spell::checkAndReturnSpellEntry(43742);
    if (sp != nullptr)
    {
        sp->custom_self_cast_only = true;
        sp->custom_ProcOnNameHash[0] = SPELL_HASH_HOLY_LIGHT;
        sp->procChance = 100;
    }

    // Recently Bandaged - is debuff
    sp = Spell::checkAndReturnSpellEntry(11196);
    if (sp != nullptr)
    {
        sp->Attributes = ATTRIBUTES_IGNORE_INVULNERABILITY;
    }

    sp = Spell::checkAndReturnSpellEntry(44856);        // Bash'ir Phasing Device
    if (sp != nullptr)
        sp->AuraInterruptFlags = AURA_INTERRUPT_ON_LEAVE_AREA;


    sp = Spell::checkAndReturnSpellEntry(24574);        // Zandalarian Hero Badge 24590 24575
    if (sp != nullptr)
    {
        sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[0] = 24590;
        sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
    }

    //Tempfix for Stone Statues
    sp = Spell::checkAndReturnSpellEntry(32253);
    if (sp != nullptr)
        sp->DurationIndex = 64;
    sp = Spell::checkAndReturnSpellEntry(32787);
    if (sp != nullptr)
        sp->DurationIndex = 64;
    sp = Spell::checkAndReturnSpellEntry(32788);
    if (sp != nullptr)
        sp->DurationIndex = 64;
    sp = Spell::checkAndReturnSpellEntry(32790);
    if (sp != nullptr)
        sp->DurationIndex = 64;
    sp = Spell::checkAndReturnSpellEntry(32791);
    if (sp != nullptr)
        sp->DurationIndex = 64;

    //////////////////////////////////////////////////////
    // GAME-OBJECT SPELL FIXES                          //
    //////////////////////////////////////////////////////

    // Blessing of Zim'Torga
    sp = Spell::checkAndReturnSpellEntry(51729);
    if (sp)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_SCRIPTED_OR_SINGLE_TARGET;
    }

    // Blessing of Zim'Abwa
    sp = Spell::checkAndReturnSpellEntry(51265);
    if (sp)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_SCRIPTED_OR_SINGLE_TARGET;
    }

    // Blessing of Zim'Rhuk
    sp = Spell::checkAndReturnSpellEntry(52051);
    if (sp)
    {
        sp->EffectImplicitTargetA[0] = EFF_TARGET_SCRIPTED_OR_SINGLE_TARGET;
    }

    // Ritual of Summoning summons a GameObject that triggers an inexistant spell.
    // This will copy an existant Summon Player spell used for another Ritual Of Summoning
    // to the one taught by Warlock trainers.
    sp = Spell::checkAndReturnSpellEntry(7720);
    if (sp)
    {
        const uint32 ritOfSummId = 62330;
        CreateDummySpell(ritOfSummId);
        SpellInfo * ritOfSumm = sSpellCustomizations.GetSpellInfo(ritOfSummId);
        if (ritOfSumm != NULL)
        {
            memcpy(ritOfSumm, sp, sizeof(SpellInfo));
            ritOfSumm->setId(ritOfSummId);
        }
    }
    //Persistent Shield
    sp = Spell::checkAndReturnSpellEntry(26467);
	if (sp)
	{
		sp->EffectTriggerSpell[0] = 26470;
		sp->Attributes |= ATTRIBUTES_NO_VISUAL_AURA | ATTRIBUTES_PASSIVE;
		sp->DurationIndex = 0;
		sp->procFlags = PROC_ON_CAST_SPELL;
	}
    //Gravity Bomb
    sp = Spell::checkAndReturnSpellEntry(63024);
	if (sp)
	{
		sp->EffectBasePoints[0] = 0;
		sp->Effect[1] = SPELL_EFFECT_NULL;
		sp->Effect[2] = SPELL_EFFECT_NULL;
		sp->TargetAuraState = 0;
		sp->casterAuraSpell = 0;
		sp->CasterAuraState = 0;
		sp->CasterAuraStateNot = 0;
		sp->TargetAuraStateNot = 0;
		sp->targetAuraSpell = 0;
		sp->casterAuraSpellNot = 0;
		sp->targetAuraSpellNot = 0;
		sp->Attributes |= ATTRIBUTES_NEGATIVE;
	}
    // War Stomp
    sp = Spell::checkAndReturnSpellEntry(20549);
    if (sp)
    {
        sp->EffectMechanic[0] = MECHANIC_STUNNED;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_ALL_ENEMY_IN_AREA;
        sp->EffectImplicitTargetB[0] = EFF_TARGET_NONE;
    }

    // Fan of knives
    sp = Spell::checkAndReturnSpellEntry(51723);
    if (sp != nullptr)
    {
        //sp->Effect[1] = SPELL_EFFECT_TRIGGER_SPELL;
        //sp->EffectTriggerSpell[1] = 52874;
        sp->EffectMechanic[0] = MECHANIC_SHACKLED;
        sp->EffectImplicitTargetA[0] = EFF_TARGET_ALL_ENEMY_IN_AREA;
        sp->EffectImplicitTargetB[0] = EFF_TARGET_NONE;
    }

    //Mage - firestarter talent ranks 1 & 2
    // overwrite procs, should only proc on these 2 spellgroups.
    sp = Spell::checkAndReturnSpellEntry(44442);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->procChance = 50;
        sp->custom_ProcOnNameHash[0] = SPELL_HASH_DRAGON_S_BREATH;
        sp->custom_ProcOnNameHash[1] = SPELL_HASH_BLAST_WAVE;
    }
    sp = Spell::checkAndReturnSpellEntry(44443);
    if (sp != nullptr)
    {
        sp->procFlags = PROC_ON_CAST_SPELL;
        sp->custom_ProcOnNameHash[0] = SPELL_HASH_DRAGON_S_BREATH;
        sp->custom_ProcOnNameHash[1] = SPELL_HASH_BLAST_WAVE;
    }
}

