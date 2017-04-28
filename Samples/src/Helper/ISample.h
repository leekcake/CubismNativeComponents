/*
 * Copyright(c) Live2D Inc. All rights reserved.
 * 
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#pragma once


// ---------------- //
// SAMPLE INTERFACE //
// ---------------- //

// Called once when app did start.
//
// @return  Should return not-zero on success; '0' otherwise.
void OnDidStart();

// Called once each frame.
//
// @param[in]  deltaTime  Delta since last tick in seconds.
void OnTick(float deltaTime);


/// Called once when app will shut down.
void OnWillQuit();
