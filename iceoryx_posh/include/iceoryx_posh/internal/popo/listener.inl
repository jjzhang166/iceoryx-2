// Copyright (c) 2021 by Apex.AI Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef IOX_POSH_POPO_LISTENER_INL
#define IOX_POSH_POPO_LISTENER_INL
namespace iox
{
namespace popo
{
namespace internal
{
template <typename T, typename UserType>
struct TranslateAndCallTypelessCallback
{
    static void call(void* const origin, void* const userType, Listener::GenericCallbackPtr_t underlyingCallback)
    {
        reinterpret_cast<Listener::CallbackWithUserTypePtr_t<T, UserType>>(underlyingCallback)(
            static_cast<T*>(origin), static_cast<UserType*>(userType));
    }
};

template <typename T>
struct TranslateAndCallTypelessCallback<T, void>
{
    static void call(void* const origin, void* const userType, Listener::GenericCallbackPtr_t underlyingCallback)
    {
        IOX_DISCARD_RESULT(userType);
        reinterpret_cast<Listener::CallbackPtr_t<T>>(underlyingCallback)(static_cast<T*>(origin));
    }
};
} // namespace internal

template <typename T>
inline cxx::expected<ListenerError> Listener::attachEvent(T& eventOrigin, CallbackRef_t<T> eventCallback) noexcept
{
    return addEvent(&eventOrigin,
                    nullptr,
                    static_cast<uint64_t>(NoEnumUsed::PLACEHOLDER),
                    typeid(NoEnumUsed).hash_code(),
                    reinterpret_cast<GenericCallbackRef_t>(eventCallback),
                    internal::TranslateAndCallTypelessCallback<T, void>::call,
                    EventAttorney::getInvalidateTriggerMethod(eventOrigin))
        .and_then([&](auto& eventId) {
            EventAttorney::enableEvent(
                eventOrigin, TriggerHandle(*m_conditionVariableData, {*this, &Listener::removeTrigger}, eventId));
        });
}

template <typename T, typename EventType, typename>
inline cxx::expected<ListenerError>
Listener::attachEvent(T& eventOrigin, const EventType eventType, CallbackRef_t<T> eventCallback) noexcept
{
    static_assert(IS_EVENT_ENUM<EventType>,
                  "Only enums with an underlying EventEnumIdentifier can be attached/detached to the Listener");
    return addEvent(&eventOrigin,
                    nullptr,
                    static_cast<uint64_t>(eventType),
                    typeid(EventType).hash_code(),
                    reinterpret_cast<GenericCallbackRef_t>(eventCallback),
                    internal::TranslateAndCallTypelessCallback<T, void>::call,
                    EventAttorney::getInvalidateTriggerMethod(eventOrigin))
        .and_then([&](auto& eventId) {
            EventAttorney::enableEvent(
                eventOrigin,
                TriggerHandle(*m_conditionVariableData, {*this, &Listener::removeTrigger}, eventId),
                eventType);
        });
}

template <typename T,
          typename EventType,
          typename UserType,
          typename = std::enable_if_t<std::is_enum<EventType>::value>>
inline cxx::expected<ListenerError> Listener::attachEvent(T& eventOrigin,
                                                          const EventType eventType,
                                                          CallbackWithUserTypeRef_t<T, UserType> eventCallback,
                                                          UserType& userType) noexcept
{
    static_assert(IS_EVENT_ENUM<EventType>,
                  "Only enums with an underlying EventEnumIdentifier can be attached/detached to the Listener");

    return addEvent(&eventOrigin,
                    &userType,
                    static_cast<uint64_t>(eventType),
                    typeid(EventType).hash_code(),
                    reinterpret_cast<GenericCallbackRef_t>(eventCallback),
                    internal::TranslateAndCallTypelessCallback<T, UserType>::call,
                    EventAttorney::getInvalidateTriggerMethod(eventOrigin))
        .and_then([&](auto& eventId) {
            EventAttorney::enableEvent(
                eventOrigin,
                TriggerHandle(*m_conditionVariableData, {*this, &Listener::removeTrigger}, eventId),
                eventType);
        });
}

template <typename T, typename UserType>
inline cxx::expected<ListenerError>
Listener::attachEvent(T& eventOrigin, CallbackWithUserTypeRef_t<T, UserType> eventCallback, UserType& userType) noexcept
{
    return addEvent(&eventOrigin,
                    &userType,
                    static_cast<uint64_t>(NoEnumUsed::PLACEHOLDER),
                    typeid(NoEnumUsed).hash_code(),
                    reinterpret_cast<GenericCallbackRef_t>(eventCallback),
                    internal::TranslateAndCallTypelessCallback<T, UserType>::call,
                    EventAttorney::getInvalidateTriggerMethod(eventOrigin))
        .and_then([&](auto& eventId) {
            EventAttorney::enableEvent(
                eventOrigin, TriggerHandle(*m_conditionVariableData, {*this, &Listener::removeTrigger}, eventId));
        });
}

template <typename T, typename UserType>
inline cxx::expected<ListenerError> Listener::attachEvent(T& eventOrigin,
                                                          const EventCallback<T, UserType>& eventCallback) noexcept
{
    return addEvent(&eventOrigin,
                    eventCallback.m_userValue,
                    static_cast<uint64_t>(NoEnumUsed::PLACEHOLDER),
                    typeid(NoEnumUsed).hash_code(),
                    reinterpret_cast<GenericCallbackRef_t>(*eventCallback.m_callback),
                    internal::TranslateAndCallTypelessCallback<T, UserType>::call,
                    EventAttorney::getInvalidateTriggerMethod(eventOrigin))
        .and_then([&](auto& eventId) {
            EventAttorney::enableEvent(
                eventOrigin, TriggerHandle(*m_conditionVariableData, {*this, &Listener::removeTrigger}, eventId));
        });
}

template <typename T, typename EventType, typename UserType, typename>
inline cxx::expected<ListenerError> Listener::attachEvent(T& eventOrigin,
                                                          const EventType eventType,
                                                          const EventCallback<T, UserType>& eventCallback) noexcept
{
    return addEvent(&eventOrigin,
                    eventCallback.m_userValue,
                    static_cast<uint64_t>(eventType),
                    typeid(EventType).hash_code(),
                    reinterpret_cast<GenericCallbackRef_t>(*eventCallback.m_callback),
                    internal::TranslateAndCallTypelessCallback<T, UserType>::call,
                    EventAttorney::getInvalidateTriggerMethod(eventOrigin))
        .and_then([&](auto& eventId) {
            EventAttorney::enableEvent(
                eventOrigin,
                TriggerHandle(*m_conditionVariableData, {*this, &Listener::removeTrigger}, eventId),
                eventType);
        });
}

template <typename T, typename EventType, typename>
inline void Listener::detachEvent(T& eventOrigin, const EventType eventType) noexcept
{
    static_assert(IS_EVENT_ENUM<EventType>,
                  "Only enums with an underlying EventEnumIdentifier can be attached/detached to the Listener");
    EventAttorney::disableEvent(eventOrigin, eventType);
}

template <typename T>
inline void Listener::detachEvent(T& eventOrigin) noexcept
{
    EventAttorney::disableEvent(eventOrigin);
}

inline constexpr uint64_t Listener::capacity() noexcept
{
    return MAX_NUMBER_OF_EVENTS_PER_LISTENER;
}


} // namespace popo
} // namespace iox
#endif
