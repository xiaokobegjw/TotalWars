local function TestEventHandler(eventData)
	print("Event Received in Lua: " .. eventData);
end

local function RegisterListener()
	RegisterEventListener(EventType.EvtData_ScriptEventTest_FromLua, TestEventHandler);
end

if next(EventType) then
	print("show event type")

	for k,v in pairs(EventType) do
		print("eventName = "..k.." eventValue ="..v)
	end
else
	print("no event")
end

RegisterListener()

QueueEvent(EventType.EvtData_ScriptEventTest_FromLua,2)