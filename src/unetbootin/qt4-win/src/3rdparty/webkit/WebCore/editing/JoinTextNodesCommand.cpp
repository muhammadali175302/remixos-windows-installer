/*
 * Copyright (C) 2005 Apple Computer, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "JoinTextNodesCommand.h"

#include "Text.h"

namespace WebCore {

JoinTextNodesCommand::JoinTextNodesCommand(Text *text1, Text *text2)
    : EditCommand(text1->document()), m_text1(text1), m_text2(text2)
{
    ASSERT(m_text1);
    ASSERT(m_text2);
    ASSERT(m_text1->nextSibling() == m_text2);
    ASSERT(m_text1->length() > 0);
    ASSERT(m_text2->length() > 0);
}

void JoinTextNodesCommand::doApply()
{
    ASSERT(m_text1);
    ASSERT(m_text2);
    ASSERT(m_text1->nextSibling() == m_text2);

    ExceptionCode ec = 0;
    m_text2->insertData(0, m_text1->data(), ec);
    ASSERT(ec == 0);

    m_text2->parentNode()->removeChild(m_text1.get(), ec);
    ASSERT(ec == 0);

    m_offset = m_text1->length();
}

void JoinTextNodesCommand::doUnapply()
{
    ASSERT(m_text2);
    ASSERT(m_offset > 0);

    ExceptionCode ec = 0;

    m_text2->deleteData(0, m_offset, ec);
    ASSERT(ec == 0);

    m_text2->parentNode()->insertBefore(m_text1.get(), m_text2.get(), ec);
    ASSERT(ec == 0);
        
    ASSERT(m_text2->previousSibling()->isTextNode());
    ASSERT(m_text2->previousSibling() == m_text1);
}

} // namespace WebCore
