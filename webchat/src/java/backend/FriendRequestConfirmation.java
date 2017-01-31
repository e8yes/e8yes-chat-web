/*
 * Copyright (C) 2017 davis
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
package backend;

import org.json.simple.JSONObject;

/**
 * @author davis
 */
public class FriendRequestConfirmation extends FriendRequest {
	
	public FriendRequestConfirmation(FriendRequest fr) {
		super(fr.sender(), fr.receiver());
	}
	
	public FriendRequestConfirmation(User sender, User receiver) {
		super(sender, receiver);
	}
	
	@Override
        public Type get_type() {
                return Notification.Type.FriendRequestConfirm;
        }
	
	@Override
        public int get_target_user()  {
                return m_sender.get_user_id();
        }
	
	@Override
        public JSONObject jsonize() {
                JSONObject o = new JSONObject();
                o.put("sender",  m_receiver.get_user_id());
		o.put("sender_alias", m_receiver.get_alias());
                o.put("receiver", m_sender.get_user_id());
		o.put("receiver_alias", m_sender.get_alias());
                return o;
        }
}
