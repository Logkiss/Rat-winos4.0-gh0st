/*--------------------------------------------------------------------------------*
 *                                                                                *
 * This material is trade secret owned by VoiceAge Corporation                    *
 * and is strictly confidential and shall remain as such.                         *
 *                                                                                *
 * Copyright © 1995-2001 VoiceAge Corporation. All Rights Reserved. No part of    *
 * this material may be reproduced, stored in a retrieval system, or transmitted, *
 * in any form or by any means, including, but not limited to, photocopying,      *
 *  electronic, mechanical, recording, or otherwise, without the prior written    *
 * permission of VoiceAge Corporation.                                            *
 *                                                                                *
 * This material is subject to continuous developments and improvements. All      *
 * warranties implied or expressed, including but not limited to implied          *
 * warranties of merchantability, or fitness for purpose, are excluded.           *
 *                                                                                *
 * ACELP and VoiceAge are registered trademark and trademark of VoiceAge          *
 * Corporation in Canada and / or other countries. Any unauthorized use is        *
 * strictly prohibited.                                                           *
 *                                                                                *
 *--------------------------------------------------------------------------------*
 *                                                                                *
 * VoiceAge Corporation                                                           *
 * 750, Chemin Lucerne                                                            *
 * Suite 250                                                                      *
 * Ville Mont-Royal (Quebec)                                                      *
 * Canada, H3R 2H6                                                                *
 *                                                                                *
 * Tel. (514) 737-4940, fax. (514) 908-2037                                       *
 *                                                                                *
 *--------------------------------------------------------------------------------*
 *
 *--------------------------------------------------------------------------------*
 *                            va_g729a.h					  *
 *                         ~~~~~~~~~~~~~~~~~~                                     *
 *--------------------------------------------------------------------------------*


/* va_g729a API functions prototypes and constants */

#define  L_FRAME_COMPRESSED 10
#define  L_FRAME            80

void va_g729a_init_encoder();
void va_g729a_encoder(short *speech, unsigned char *bitstream);
void va_g729a_init_decoder();
void va_g729a_decoder(unsigned char *bitstream, short *synth_short, int bfi);
